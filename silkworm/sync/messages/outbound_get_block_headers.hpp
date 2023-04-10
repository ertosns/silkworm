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

#pragma once

#include <silkworm/sync/packets/get_block_headers_packet.hpp>

#include "outbound_message.hpp"

namespace silkworm {

class OutboundGetBlockHeaders : public OutboundMessage {
  public:
    OutboundGetBlockHeaders();

    std::string name() const override { return "OutboundGetBlockHeaders"; }
    std::string content() const override;

    void execute(db::ROAccess, HeaderChain&, BodySequence&, SentryClient&) override;

    GetBlockHeadersPacket66& packet();
    std::vector<PeerPenalization>& penalties();
    bool packet_present() const;

  private:
    ::sentry::SentPeers send_packet(SentryClient&, seconds_t timeout);
    void send_penalization(SentryClient&, const PeerPenalization&, seconds_t timeout);

    GetBlockHeadersPacket66 packet_{};
    std::vector<PeerPenalization> penalizations_;
};

}  // namespace silkworm
