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

#include <silkworm/sync/packets/block_bodies_packet.hpp>

#include "outbound_message.hpp"

namespace silkworm {

class OutboundBlockBodies : public OutboundMessage {
  public:
    explicit OutboundBlockBodies(BlockBodiesPacket66 packet) : packet_(std::move(packet)) {}

    [[nodiscard]] std::string name() const override { return "OutboundBlockBodies"; }
    [[nodiscard]] std::string content() const override;

    void execute(db::ROAccess, HeaderChain&, BodySequence&, SentryClient&) override;

    [[nodiscard]] silkworm::sentry::eth::MessageId eth_message_id() const override {
        return silkworm::sentry::eth::MessageId::kBlockBodies;
    }

    [[nodiscard]] Bytes message_data() const override;

  private:
    BlockBodiesPacket66 packet_{};
};

}  // namespace silkworm
