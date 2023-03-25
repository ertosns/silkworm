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

#include "peer_id.hpp"

#include <silkworm/infra/rpc/interfaces/types.hpp>

namespace silkworm::sentry::rpc::interfaces {

namespace proto_types = ::types;

sentry::common::EccPublicKey peer_public_key_from_id(const ::types::H512& peer_id) {
    return sentry::common::EccPublicKey::deserialize(bytes_from_H512(peer_id));
}

proto_types::H512 peer_id_from_public_key(const sentry::common::EccPublicKey& key) {
    return *H512_from_bytes(key.serialized());
}

sentry::common::EccPublicKey peer_public_key_from_id_string(const std::string& peer_id_str) {
    return sentry::common::EccPublicKey::deserialize_hex(peer_id_str);
}

std::string peer_id_string_from_public_key(const sentry::common::EccPublicKey& key) {
    return key.hex();
}

}  // namespace silkworm::sentry::rpc::interfaces
