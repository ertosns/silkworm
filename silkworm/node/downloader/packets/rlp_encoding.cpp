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

// clang-format off
#include <silkworm/node/downloader/internals/types.hpp>  // types
// clang-format on

#include <silkworm/core/rlp/encode_vector.hpp>  // generic implementations (must follow types)

#include "block_bodies_packet.hpp"
#include "block_headers_packet.hpp"
#include "get_block_bodies_packet.hpp"
#include "get_block_headers_packet.hpp"
#include "new_block_hashes_packet.hpp"
#include "new_block_packet.hpp"
#include "rlp_eth66_packet_coding.hpp"

namespace silkworm::rlp {

void encode(Bytes& to, const Hash& h) { rlp::encode(to, ByteView{h}); }

size_t length(const BlockBodiesPacket66& from) noexcept { return rlp::length_eth66_packet(from); }

void encode(Bytes& to, const BlockBodiesPacket66& from) { return rlp::encode_eth66_packet(to, from); }

size_t length(const BlockHeadersPacket66& from) noexcept { return rlp::length_eth66_packet(from); }

void encode(Bytes& to, const BlockHeadersPacket66& from) { return rlp::encode_eth66_packet(to, from); }

size_t length(const GetBlockBodiesPacket66& from) noexcept { return rlp::length_eth66_packet(from); }

void encode(Bytes& to, const GetBlockBodiesPacket66& from) { return rlp::encode_eth66_packet(to, from); }

// NewBlockHash
size_t length(const NewBlockHash& from) noexcept {
    rlp::Header rlp_head{true, rlp::length(from.hash) + rlp::length(from.number)};

    size_t rlp_head_len = rlp::length_of_length(rlp_head.payload_length);
    return rlp_head_len + rlp_head.payload_length;
}

void encode(Bytes& to, const NewBlockHash& from) noexcept {
    rlp::Header rlp_head{true, rlp::length(from.hash) + rlp::length(from.number)};

    rlp::encode_header(to, rlp_head);

    rlp::encode(to, from.hash);
    rlp::encode(to, from.number);
}

// NewBlockPacket
void encode(Bytes& to, const NewBlockPacket& from) noexcept {
    rlp::Header rlp_head{true, rlp::length(from.block) + rlp::length(from.td)};

    rlp::encode_header(to, rlp_head);

    rlp::encode(to, from.block);
    rlp::encode(to, from.td);
}

size_t length(const NewBlockPacket& from) noexcept {
    rlp::Header rlp_head{true, rlp::length(from.block) + rlp::length(from.td)};

    size_t rlp_head_len = rlp::length_of_length(rlp_head.payload_length);
    return rlp_head_len + rlp_head.payload_length;
}

// GetBlockHeadersPacket
size_t length(const GetBlockHeadersPacket& from) noexcept {
    rlp::Header rlp_head{true, 0};

    rlp_head.payload_length += rlp::length(from.origin);
    rlp_head.payload_length += rlp::length(from.amount);
    rlp_head.payload_length += rlp::length(from.skip);
    rlp_head.payload_length += rlp::length(from.reverse);

    size_t rlp_head_len = rlp::length_of_length(rlp_head.payload_length);

    return rlp_head_len + rlp_head.payload_length;
}

void encode(Bytes& to, const GetBlockHeadersPacket& from) noexcept {
    rlp::Header rlp_head{true, 0};

    rlp_head.payload_length += rlp::length(from.origin);
    rlp_head.payload_length += rlp::length(from.amount);
    rlp_head.payload_length += rlp::length(from.skip);
    rlp_head.payload_length += rlp::length(from.reverse);

    rlp::encode_header(to, rlp_head);

    rlp::encode(to, from.origin);
    rlp::encode(to, from.amount);
    rlp::encode(to, from.skip);
    rlp::encode(to, from.reverse);
}

size_t length(const GetBlockHeadersPacket66& from) noexcept { return rlp::length_eth66_packet(from); }

void encode(Bytes& to, const GetBlockHeadersPacket66& from) noexcept { return rlp::encode_eth66_packet(to, from); }

}  // namespace silkworm::rlp
