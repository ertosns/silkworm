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

#include <silkworm/infra/concurrency/task.hpp>

#include <boost/asio/io_context.hpp>

#include <silkworm/core/types/block.hpp>
#include <silkworm/node/stagedsync/types.hpp>

namespace silkworm::execution {

namespace asio = boost::asio;

class Client {
  public:
    virtual ~Client() = default;

    virtual asio::io_context& get_executor() = 0;

    // actions
    virtual auto insert_headers(const BlockVector& blocks) -> Task<void> = 0;
    virtual auto insert_bodies(const BlockVector& blocks) -> Task<void> = 0;
    virtual auto insert_blocks(const BlockVector& blocks) -> Task<void> = 0;

    virtual auto validate_chain(Hash head_block_hash) -> Task<ValidationResult> = 0;

    virtual auto update_fork_choice(Hash head_block_hash, std::optional<Hash> finalized_block_hash = std::nullopt)
        -> Task<ForkChoiceApplication> = 0;

    // state
    virtual auto block_progress() -> Task<BlockNum> = 0;
    virtual auto last_fork_choice() -> Task<BlockId> = 0;

    // header/body retrieval
    virtual auto get_header(Hash block_hash) -> Task<std::optional<BlockHeader>> = 0;
    virtual auto get_header(BlockNum height, Hash hash) -> Task<std::optional<BlockHeader>> = 0;
    virtual auto get_body(Hash block_hash) -> Task<std::optional<BlockBody>> = 0;
    virtual auto get_body(BlockNum block_number) -> Task<std::optional<BlockBody>> = 0;

    virtual auto is_canonical(Hash block_hash) -> Task<bool> = 0;
    virtual auto get_block_num(Hash block_hash) -> Task<std::optional<BlockNum>> = 0;

    virtual auto get_last_headers(BlockNum limit) -> Task<std::vector<BlockHeader>> = 0;
    virtual auto get_header_td(Hash, std::optional<BlockNum>) -> Task<std::optional<TotalDifficulty>> = 0;
};

}  // namespace silkworm::execution
