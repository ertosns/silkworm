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

#include <cstdint>
#include <memory>
#include <optional>

#include <silkworm/core/common/bytes.hpp>
#include <silkworm/db/etl/collector.hpp>
#include <silkworm/infra/common/memory_mapped_file.hpp>

#include "index_builder.hpp"
#include "path.hpp"

namespace silkworm::snapshots {

struct TransactionKeyFactory : IndexKeyFactory {
    TransactionKeyFactory(uint64_t first_tx_id) : first_tx_id_(first_tx_id) {}
    ~TransactionKeyFactory() override = default;

    Bytes make(ByteView key_data, uint64_t i) override;

  private:
    uint64_t first_tx_id_;
};

class TransactionIndex {
  public:
    static IndexBuilder make(
        SnapshotPath bodies_segment_path,
        SnapshotPath segment_path) {
        return make(
            std::move(bodies_segment_path), std::nullopt,
            std::move(segment_path), std::nullopt);
    }

    static IndexBuilder make(
        SnapshotPath bodies_segment_path,
        std::optional<MemoryMappedRegion> bodies_segment_region,
        SnapshotPath segment_path,
        std::optional<MemoryMappedRegion> segment_region) {
        auto txs_amount = compute_txs_amount(std::move(bodies_segment_path), bodies_segment_region);
        auto descriptor = make_descriptor(segment_path, txs_amount.first);
        auto query = std::make_unique<DecompressorIndexInputDataQuery>(std::move(segment_path), segment_region);
        return IndexBuilder{std::move(descriptor), std::move(query)};
    }

    static SnapshotPath bodies_segment_path(const SnapshotPath& segment_path);
    static std::pair<uint64_t, uint64_t> compute_txs_amount(
        SnapshotPath bodies_segment_path,
        std::optional<MemoryMappedRegion> bodies_segment_region);

  private:
    static IndexDescriptor make_descriptor(const SnapshotPath& segment_path, uint64_t first_tx_id) {
        return {
            .index_file = segment_path.index_file(),
            .key_factory = std::make_unique<TransactionKeyFactory>(first_tx_id),
            .base_data_id = first_tx_id,
            .less_false_positives = true,
            .etl_buffer_size = db::etl::kOptimalBufferSize / 2,
        };
    }
};

}  // namespace silkworm::snapshots
