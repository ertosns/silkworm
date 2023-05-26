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

#include <silkworm/sync/internals/types.hpp>

namespace silkworm {

struct NewBlockPacket {
    Block block;
    BigInt td;  // total difficulty
};

namespace rlp {

    void encode(Bytes& to, const NewBlockPacket& from) noexcept;

    size_t length(const NewBlockPacket& from) noexcept;

    DecodingResult decode(ByteView& from, NewBlockPacket& to, Leftover mode = Leftover::kProhibit) noexcept;

}  // namespace rlp

inline std::ostream& operator<<(std::ostream& os, const NewBlockPacket& packet) {
    os << "block num " << packet.block.header.number;
    return os;
}

}  // namespace silkworm
