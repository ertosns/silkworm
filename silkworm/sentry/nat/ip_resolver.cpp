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

#include "ip_resolver.hpp"

#include "local_ip_resolver.hpp"
#include "stun_ip_resolver.hpp"

namespace silkworm::sentry::nat {

Task<boost::asio::ip::address> ip_resolver(const NatOption& option) {
    switch (option.mode) {
        case NatMode::kNone:
            co_return (co_await local_ip_resolver());
        case NatMode::kExternalIP:
            co_return option.value.value();
        case NatMode::kStun:
            co_return (co_await stun_ip_resolver());
    }
}

}  // namespace silkworm::sentry::nat
