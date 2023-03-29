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

#include "sha256.hpp"

#include <silkworm/core/crypto/sha256.h>

namespace silkworm::sentry::rlpx::crypto {

Bytes sha256(ByteView data) {
    Bytes hash(32, 0);
    silkworm_sha256(hash.data(), data.data(), data.size(), /* use_cpu_extensions = */ false);
    return hash;
}

}  // namespace silkworm::sentry::rlpx::crypto
