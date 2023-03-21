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

#include "peer_event.hpp"

#include "peer_id.hpp"

namespace silkworm::sentry::rpc::interfaces {

namespace proto = ::sentry;

api::api_common::PeerEvent peer_event_from_proto_peer_event(const proto::PeerEvent& event) {
    api::api_common::PeerEventId event_id;
    switch (event.event_id()) {
        case proto::PeerEvent_PeerEventId_Connect:
            event_id = api::api_common::PeerEventId::kAdded;
            break;
        case proto::PeerEvent_PeerEventId_Disconnect:
            event_id = api::api_common::PeerEventId::kRemoved;
            break;
        default:
            event_id = api::api_common::PeerEventId::kRemoved;  // Avoid -Wsometimes-uninitialized
            assert(false);
    }

    return api::api_common::PeerEvent{
        {peer_public_key_from_id(event.peer_id())},
        event_id,
    };
}

proto::PeerEvent proto_peer_event_from_peer_event(const api::api_common::PeerEvent& event) {
    proto::PeerEvent reply;
    if (event.peer_public_key) {
        reply.mutable_peer_id()->CopyFrom(peer_id_from_public_key(event.peer_public_key.value()));
    }
    switch (event.event_id) {
        case api::api_common::PeerEventId::kAdded:
            reply.set_event_id(proto::PeerEvent_PeerEventId_Connect);
            break;
        case api::api_common::PeerEventId::kRemoved:
            reply.set_event_id(proto::PeerEvent_PeerEventId_Disconnect);
            break;
    }
    return reply;
}

}  // namespace silkworm::sentry::rpc::interfaces
