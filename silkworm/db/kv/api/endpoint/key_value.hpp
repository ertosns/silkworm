/*
   Copyright 2024 The Silkworm Authors

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

#include <tuple>

#include <silkworm/core/common/bytes.hpp>

namespace silkworm::db::kv::api {

struct KeyValue {
    Bytes key;
    Bytes value;

    KeyValue() noexcept = default;

    KeyValue(Bytes k, Bytes v) : key{std::move(k)}, value{std::move(v)} {}

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    KeyValue(Bytes k) : key{std::move(k)} {}

    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    KeyValue(std::pair<Bytes, Bytes> kv_pair)
        : key{std::move(kv_pair.first)}, value{std::move(kv_pair.second)} {}
};

inline bool operator<(const KeyValue& lhs, const KeyValue& rhs) {
    return lhs.key < rhs.key;
}

inline bool operator==(const KeyValue& lhs, const KeyValue& rhs) {
    return lhs.key == rhs.key;
}

}  // namespace silkworm::db::kv::api
