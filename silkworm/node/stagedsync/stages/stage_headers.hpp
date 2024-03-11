/*
   Copyright 2022 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#pragma once

#include <atomic>

#include <silkworm/core/types/hash.hpp>
#include <silkworm/infra/common/measure.hpp>
#include <silkworm/node/db/access_layer.hpp>
#include <silkworm/node/db/stage.hpp>

namespace silkworm::stagedsync {

/*
 * HeadersStage implement the header downloading stage.
 * Like the other stages it has two methods, one to go forward and one to go backwards (unwind) in the chain.
 * It is the counterpart of Erigon's HeaderForward and HeadersUnwind.
 *
 * HeadersStage internally uses HeaderChain and HeaderPersistence. The first represent the growing chain in memory
 * the second represent the growing chain on db. When headers are ready to be persisted they are withdrawn from
 * HeaderChain and transferred to the HeaderPersistence that write them on the db.
 *
 * HeadersStage has:
 *    - an execution loop
 *    - a forward/unwind method pair
 *
 *  The execution loop process messages that arrives in a concurrent queue. They are of 2 types:
 *    - inbound messages (from peers), like headers announcements (that we receive), responses for our header requests
 *    - outbound messages (to peers), like headers announcements (that we propagate), header requests
 *  Each message is class that encode in the execute() method the logic to process the message itself (command pattern).
 *  Those messages operate on the HeaderChain (asking needed headers or providing arrived headers) and use the sentry.
 *  Inbound messages are generated by the sentry, outbound messages are generated by the forward/unwind methods.
 *
 *  The forward method periodically
 *     - generate some outbound messages that ask HeaderChain to generate headers request,
 *     - check if HeaderChain has headers ready to be persisted
 *     - if there are some it uses HeaderPersistence to persist them
 *     - check if HeaderPersistence has detected an unwind point
 *     - check the conditions that determines the forward method to exit (unwind detected, chain in sync)
 *  The unwind method do headers unwind down to an unwind point.
 *
 *  Since the execution loop runs in its thread and the forward/unwind methods runs in the stage loop thread a
 *  synchronisation is needed. So the HeadersStage is partitioned in 2 half: one half runs the execution loop, the
 *  other half runs the forward/unwind methods; the two half communicate only by a MessageQueue that is a thread safe
 *  queue. Thus, HeaderChain is used only in the first half and not need lock protection, whereas HeaderPersistence is
 *  used in the other thread and also do not need lock protection.
 *
 */
class HeadersStage : public Stage {
  public:
    HeadersStage(SyncContext*);
    HeadersStage(const HeadersStage&) = delete;  // not copyable
    HeadersStage(HeadersStage&&) = delete;       // nor movable

    Stage::Result forward(db::RWTxn&) override;  // go forward, downloading headers
    Stage::Result unwind(db::RWTxn&) override;   // go backward, unwinding headers to new_height
    Stage::Result prune(db::RWTxn&) override;

  protected:
    std::vector<std::string> get_log_progress() override;  // thread safe
    std::atomic<BlockNum> current_height_{0};

    std::optional<BlockNum> forced_target_block_;

    // HeaderDataModel has the responsibility to update headers related tables
    class HeaderDataModel {
      public:
        explicit HeaderDataModel(db::RWTxn& tx, BlockNum headers_height);

        void update_tables(const BlockHeader&);  // update header related tables

        // remove header data from tables, used in unwind phase
        static void remove_headers(BlockNum unwind_point, db::RWTxn& tx);

        // holds the status of a batch insertion of headers
        [[nodiscard]] BlockNum highest_height() const;
        [[nodiscard]] Hash highest_hash() const;
        [[nodiscard]] intx::uint256 total_difficulty() const;

        std::optional<BlockHeader> get_canonical_header(BlockNum height) const;

      private:
        db::RWTxn& tx_;
        db::DataModel data_model_;
        Hash previous_hash_;
        intx::uint256 previous_td_{0};
        BlockNum previous_height_{0};
    };
};

}  // namespace silkworm::stagedsync
