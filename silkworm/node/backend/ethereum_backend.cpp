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

#include "ethereum_backend.hpp"

namespace silkworm {

EthereumBackEnd::EthereumBackEnd(
    const NodeSettings& node_settings,
    mdbx::env* chaindata_env,
    std::shared_ptr<sentry::api::api_common::SentryClient> sentry_client)
    : EthereumBackEnd(node_settings, chaindata_env, std::move(sentry_client), std::make_unique<StateChangeCollection>()) {
}

EthereumBackEnd::EthereumBackEnd(
    const NodeSettings& node_settings,
    mdbx::env* chaindata_env,
    std::shared_ptr<sentry::api::api_common::SentryClient> sentry_client,
    std::unique_ptr<StateChangeCollection> state_change_collection)
    : node_settings_(node_settings),
      chaindata_env_(chaindata_env),
      sentry_client_(std::move(sentry_client)),
      state_change_collection_(std::move(state_change_collection)) {
    // Get the numeric chain identifier from node settings
    if (node_settings_.chain_config) {
        chain_id_ = (*node_settings_.chain_config).chain_id;
    }
}

void EthereumBackEnd::set_node_name(const std::string& node_name) noexcept {
    node_name_ = node_name;
}

void EthereumBackEnd::close() {
    state_change_collection_->close();
}

}  // namespace silkworm
