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

#include <silkworm/db/snapshots/snapshot_reader.hpp>
#include <silkworm/db/snapshots/snapshot_writer.hpp>

#include "txn_snapshot_word_serializer.hpp"

namespace silkworm::snapshots {

using TransactionSnapshotReader = SnapshotReader<TransactionSnapshotWordDeserializer>;
using TransactionSnapshotWriter = SnapshotWriter<TransactionSnapshotWordSerializer>;

template <BytesOrByteView TBytes>
using TransactionSnapshotPayloadRlpReader = SnapshotReader<TransactionSnapshotWordPayloadRlpDeserializer<TBytes>>;

}  // namespace silkworm::snapshots
