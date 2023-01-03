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

#include "protocol.hpp"

namespace silkworm::sentry::eth {

const uint8_t Protocol::kVersion = 67;

void Protocol::handle_peer_first_message(const common::Message& message) {
    if (message.id != StatusMessage::kId)
        throw std::runtime_error("eth::Protocol: unexpected first message");

    auto peer_status = StatusMessage::from_message(message);
    auto my_status = status_provider_();

    bool is_compatible =
        (peer_status.version == kVersion) &&
        (peer_status.network_id == my_status.message.network_id) &&
        (peer_status.genesis_hash == my_status.message.genesis_hash) &&
        peer_status.fork_id.is_compatible_with(
            my_status.message.genesis_hash,
            my_status.fork_block_numbers,
            my_status.head_block_num);

    if (!is_compatible)
        throw Protocol::IncompatiblePeerError();
}

}  // namespace silkworm::sentry::eth
