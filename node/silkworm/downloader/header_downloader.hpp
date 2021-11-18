/*
   Copyright 2021 The Silkworm Authors

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
#ifndef SILKWORM_HEADER_DOWNLOADER_HPP
#define SILKWORM_HEADER_DOWNLOADER_HPP

#include <atomic>

#include <silkworm/chain/identity.hpp>
#include <silkworm/concurrency/active_component.hpp>
#include <silkworm/concurrency/containers.hpp>

#include "internals/db_tx.hpp"
#include "internals/types.hpp"
#include "internals/working_chain.hpp"
#include "messages/InternalMessage.hpp"
#include "sentry_client.hpp"

namespace silkworm {

// (proposed) abstract interface for all stages
class Stage {
  public:
    struct Result {
        enum Status { Unknown, Done, DoneAndUpdated, UnwindNeeded, SkipTx, Error } status;
        std::optional<BlockNum> current_point; // todo: do we need this?
        std::optional<BlockNum> unwind_point;
    };

    virtual Result forward(bool first_sync) = 0;
    virtual Result unwind_to(BlockNum new_height, Hash bad_block) = 0;
};

// custom exception
class HeaderDownloaderException : public std::runtime_error {
  public:
    explicit HeaderDownloaderException(const std::string& cause) : std::runtime_error(cause) {}
};

/*
 * HeaderDownloader implement the header downloading stage.
 * Like the other stages it has two methods, one to go forward and one to go backwards (unwind) in the chain.
 * It is the counterpart of Erigon's HeaderForward and HeadersUnwind.
 *
 * HeaderDownloader internally uses WorkingChain and PersistedChain. The first represent the growing chain in memory
 * the second represent the growing chain on db. When headers are ready to be persisted they are withdrawn from
 * WorkingChain and transferred to the PersistedChain that write them on the db.
 *
 * HeaderDownloader has:
 *    - an execution loop
 *    - a forward/unwind method pair
 *
 *  The execution loop process messages that arrives in a concurrent queue. They are of 2 types:
 *    - inbound messages (from peers), like headers announcements (that we receive), responses for our header requests
 *    - outbound messages (to peers), like headers announcements (that we propagate), header requests
 *  Each message is class that encode in the execute() method the logic to process the message itself (command pattern).
 *  Those messages operate on the WorkingChain (asking needed headers or providing arrived headers) and use the sentry.
 *  Inbound messages are generated by the sentry, outbound messages are generated by the forward/unwind methods.
 *
 *  The forward method periodically
 *     - generate some outbound messages that ask WorkingChain to generate headers request,
 *     - check if WorkingChain has headers ready to be persisted
 *     - if there are some it uses PersistedChain to persist them
 *     - check if PersistedChain has detected an unwind point
 *     - check the conditions that determines the forward method to exit (unwind detected, chain in sync)
 *  The unwind method do headers unwind down to an unwind point.
 *
 *  Since the execution loop runs in its thread and the forward/unwind methods runs in the stage loop thread a
 *  synchronisation is needed. So the HeaderDownloader is partitioned in 2 half: one half runs the execution loop, the
 *  other half runs the forward/unwind methods; the two half communicate only by a MessageQueue that is a thread safe
 *  queue. Thus, WorkingChain is used only in the first half and not need lock protection, whereas PersistedChain is
 *  used in the other thread and also do not need lock protection.
 *
 */
class HeaderDownloader : public Stage, public ActiveComponent {
    ChainIdentity chain_identity_;
    Db::ReadWriteAccess db_access_;
    SentryClient& sentry_;

  public:
    HeaderDownloader(SentryClient& sentry, Db::ReadWriteAccess db_access, ChainIdentity chain_identity);
    HeaderDownloader(const HeaderDownloader&) = delete;  // not copyable
    HeaderDownloader(HeaderDownloader&&) = delete;       // nor movable
    ~HeaderDownloader();

    Stage::Result forward(bool first_sync) override;  // go forward, downloading headers
    Stage::Result unwind_to(BlockNum new_height,
                            Hash bad_block = {}) override;  // go backward, unwinding headers to new_height

    /*[[long_running]]*/ void receive_messages();  // subscribe with sentry to receive messages
                                                   // and do a long-running loop to wait for messages

    /*[[long_running]]*/ void execution_loop() override;  // process messages popping them from the queue

  private:
    using MessageQueue = ConcurrentQueue<std::shared_ptr<Message>>;  // used internally to store new messages

    void send_status();           // send chain identity to sentry
    void send_header_requests();  // send requests for more headers
    void send_announcements();
    auto sync_working_chain(BlockNum highest_in_db) -> std::shared_ptr<InternalMessage<void>>;
    auto withdraw_stable_headers() -> std::shared_ptr<InternalMessage<std::tuple<Headers, bool>>>;
    auto update_bad_headers(std::set<Hash>) -> std::shared_ptr<InternalMessage<void>>;

    WorkingChain working_chain_;
    MessageQueue messages_{};  // thread safe queue where to receive messages from sentry

    // todo: put a barrier around WorkingChain & MessageQueue using this class
    /* Background_Processing runs forever and processes messages arriving from the outside (peers) via Sentry;
     * Messages carries data & code to update the WorkingChain so Background_Processing only responsibility is
     * to provide a thead and to put a barrier around WorkingChain enforcing that it is accessed only in this thread.
     * It also communicates with the downloader using the same mechanism: downloader creates a message and put it in the
     * MessageQueue waiting for message processing (and results).
     /
    class Background_Processing {
        MessageQueue messages;
        WorkingChain working_chain_;
        IConsensusEngine consensus_engine_;
      public:
        void receive_message(shared_ptr<Message>); // put message in the queue; call it from sentry (pub/sub) and from
                                                   // the downloader

        [[long_running]] void process_messages(); // wait for a message, pop and process it; provide a thread from the
                                                   // outside
    };
    */
};

}  // namespace silkworm

#endif  // SILKWORM_HEADER_DOWNLOADER_HPP
