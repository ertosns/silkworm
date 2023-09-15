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

#include <chrono>
#include <map>

#include <silkworm/core/common/bytes.hpp>
#include <silkworm/sentry/common/ecc_public_key.hpp>
#include <silkworm/sentry/discovery/common/node_address.hpp>

namespace silkworm::sentry::discovery::disc_v4::find {

struct NeighborsMessage {
    std::map<EccPublicKey, NodeAddress> node_addresses;
    std::chrono::time_point<std::chrono::system_clock> expiration;

    [[nodiscard]] Bytes rlp_encode() const;
    [[nodiscard]] static NeighborsMessage rlp_decode(ByteView data);

    static const uint8_t kId;
};

}  // namespace silkworm::sentry::discovery::disc_v4::find
