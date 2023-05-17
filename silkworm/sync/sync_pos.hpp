/*
   Copyright 2023 The Silkworm Authors

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

#include <silkworm/infra/concurrency/coroutine.hpp>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

#include <silkworm/infra/common/log.hpp>
#include <silkworm/infra/concurrency/active_component.hpp>
#include <silkworm/node/common/settings.hpp>
#include <silkworm/node/stagedsync/client.hpp>
#include <silkworm/silkrpc/types/execution_payload.hpp>
#include <silkworm/sync/internals/chain_fork_view.hpp>
#include <silkworm/sync/messages/internal_message.hpp>

#include "block_exchange.hpp"
#include "chain_sync.hpp"

namespace silkworm::chainsync {

namespace asio = boost::asio;

class PoSSync : public ChainSync {
  public:
    PoSSync(BlockExchange&, execution::Client&);

    asio::awaitable<void> async_run() override;

    // public interface to download blocks
    auto download_blocks() -> asio::awaitable<void>; /*[[long_running]]*/

    // public interface called by the external PoS client
    auto new_payload_v1(const rpc::ExecutionPayloadV1&) -> asio::awaitable<rpc::PayloadStatusV1>;
    auto fork_choice_update_v1(const rpc::ForkChoiceStateV1&, const std::optional<rpc::PayloadAttributesV1>&) -> asio::awaitable<rpc::ForkChoiceUpdatedReplyV1>;
    auto get_payload_v1(uint64_t payloadId) -> asio::awaitable<rpc::ExecutionPayloadV1>;

  private:
    static auto make_execution_block(const rpc::ExecutionPayloadV1& payload) -> std::shared_ptr<Block>;
    void do_sanity_checks(const BlockHeader& header, TotalDifficulty parent_td);
    // bool extends_canonical(const Block& block, Hash block_hash);
    auto has_bad_ancestor(const Hash& block_hash) -> std::tuple<bool, Hash>;
};

}  // namespace silkworm::chainsync
