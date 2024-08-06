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

#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>

#include <silkworm/infra/concurrency/task.hpp>

#include <silkworm/db/mdbx/mdbx.hpp>

#include "base_transaction.hpp"
#include "cursor.hpp"
#include "local_cursor.hpp"
#include "state_cache.hpp"

namespace silkworm::db::kv::api {

class LocalTransaction : public BaseTransaction {
  public:
    explicit LocalTransaction(mdbx::env chaindata_env, StateCache* state_cache)
        : BaseTransaction(state_cache), chaindata_env_{std::move(chaindata_env)}, txn_{chaindata_env_} {}

    ~LocalTransaction() override = default;

    [[nodiscard]] uint64_t tx_id() const override { return tx_id_; }
    [[nodiscard]] uint64_t view_id() const override { return txn_.id(); }

    Task<void> open() override;

    Task<std::shared_ptr<Cursor>> cursor(const std::string& table) override;

    Task<std::shared_ptr<CursorDupSort>> cursor_dup_sort(const std::string& table) override;

    std::shared_ptr<State> create_state(boost::asio::any_io_executor& executor, const chain::ChainStorage& storage, BlockNum block_number) override;

    std::shared_ptr<chain::ChainStorage> create_storage() override;

    Task<void> close() override;

    // rpc HistoryGet(HistoryGetReq) returns (HistoryGetReply);
    Task<HistoryPointResult> history_seek(api::HistoryPointQuery&& query) override;

    // rpc IndexRange(IndexRangeReq) returns (IndexRangeReply);
    Task<PaginatedTimestamps> index_range(IndexRangeQuery&& query) override;

    // rpc HistoryRange(HistoryRangeReq) returns (Pairs);
    Task<PaginatedKeysValues> history_range(HistoryRangeQuery&& query) override;

    // rpc DomainRange(DomainRangeReq) returns (Pairs);
    Task<PaginatedKeysValues> domain_range(DomainRangeQuery&& query) override;

  private:
    Task<std::shared_ptr<CursorDupSort>> get_cursor(const std::string& table, bool is_cursor_dup_sort);

    static inline uint64_t next_tx_id_{0};

    std::map<std::string, std::shared_ptr<CursorDupSort>> cursors_;
    std::map<std::string, std::shared_ptr<CursorDupSort>> dup_cursors_;

    mdbx::env chaindata_env_;
    uint32_t last_cursor_id_{0};
    ROTxnManaged txn_;
    uint64_t tx_id_{++next_tx_id_};
};

}  // namespace silkworm::db::kv::api
