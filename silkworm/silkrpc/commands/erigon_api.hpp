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

#include <memory>

#include <silkworm/infra/concurrency/coroutine.hpp>

#include <boost/asio/awaitable.hpp>
#include <nlohmann/json.hpp>

#include <silkworm/silkrpc/concurrency/context_pool.hpp>
#include <silkworm/silkrpc/core/rawdb/accessors.hpp>
#include <silkworm/silkrpc/ethdb/database.hpp>
#include <silkworm/silkrpc/ethdb/kv/state_cache.hpp>
#include <silkworm/silkrpc/json/types.hpp>

namespace silkworm::http {
class RequestHandler;
}

namespace silkworm::rpc::commands {

class ErigonRpcApi {
  public:
    explicit ErigonRpcApi(Context& context);
    virtual ~ErigonRpcApi() = default;

    ErigonRpcApi(const ErigonRpcApi&) = delete;
    ErigonRpcApi& operator=(const ErigonRpcApi&) = delete;

  protected:
    boost::asio::awaitable<void> handle_erigon_block_number(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_get_block_by_timestamp(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_get_header_by_hash(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_get_header_by_number(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_get_logs_by_hash(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_forks(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_watch_the_burn(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_cumulative_chain_traffic(const nlohmann::json& request, nlohmann::json& reply);
    boost::asio::awaitable<void> handle_erigon_node_info(const nlohmann::json& request, nlohmann::json& reply);

  private:
    Context& context_;
    std::unique_ptr<ethbackend::BackEnd>& backend_;
    std::shared_ptr<BlockCache>& block_cache_;
    std::unique_ptr<ethdb::Database>& database_;

    friend class silkworm::http::RequestHandler;
};

}  // namespace silkworm::rpc::commands
