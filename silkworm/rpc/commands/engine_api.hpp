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

#include <silkworm/infra/concurrency/task.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/thread_pool.hpp>
#include <nlohmann/json.hpp>
#include <tl/expected.hpp>

#include <silkworm/infra/concurrency/private_service.hpp>
#include <silkworm/rpc/ethbackend/backend.hpp>
#include <silkworm/rpc/ethdb/database.hpp>
#include <silkworm/rpc/json/types.hpp>

namespace silkworm::rpc::json_rpc {
class RequestHandler;
}

namespace silkworm::rpc::commands {

class EngineRpcApi {
  public:
    EngineRpcApi(ethdb::Database* database, ethbackend::BackEnd* backend)
        : database_{database}, backend_{backend} {}
    explicit EngineRpcApi(boost::asio::io_context& io_context)
        : EngineRpcApi(
              must_use_private_service<ethdb::Database>(io_context),
              must_use_private_service<ethbackend::BackEnd>(io_context)) {}
    virtual ~EngineRpcApi() = default;

    EngineRpcApi(const EngineRpcApi&) = delete;
    EngineRpcApi& operator=(const EngineRpcApi&) = delete;
    EngineRpcApi(EngineRpcApi&&) = default;

  protected:
    Task<void> handle_engine_exchange_capabilities(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_get_payload_v1(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_get_payload_v2(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_get_payload_v3(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_get_payload_bodies_by_hash_v1(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_get_payload_bodies_by_range_v1(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_new_payload_v1(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_new_payload_v2(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_new_payload_v3(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_forkchoice_updated_v1(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_forkchoice_updated_v2(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_forkchoice_updated_v3(const nlohmann::json& request, nlohmann::json& reply);
    Task<void> handle_engine_exchange_transition_configuration_v1(const nlohmann::json& request, nlohmann::json& reply);

  private:
    // TODO(canepat) remove this method and pass ChainConfig as constructor parameter
    Task<std::optional<silkworm::ChainConfig>> read_chain_config();

    using ApiError = std::pair<int, std::string>;
    using ValidationError = tl::expected<void, ApiError>;

    ValidationError validate_fork_choice_state_v1(const ForkChoiceState& state);

    ValidationError validate_payload_attributes_v2(const std::optional<PayloadAttributes>& attributes,
                                                   const ForkChoiceUpdatedReply& reply,
                                                   const std::optional<silkworm::ChainConfig>& config);
    ValidationError validate_payload_attributes_v3(const std::optional<PayloadAttributes>& attributes,
                                                   const ForkChoiceUpdatedReply& reply,
                                                   const std::optional<silkworm::ChainConfig>& config);

    ethdb::Database* database_;
    ethbackend::BackEnd* backend_;

    friend class silkworm::rpc::json_rpc::RequestHandler;
};

}  // namespace silkworm::rpc::commands
