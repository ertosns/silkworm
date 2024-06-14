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

#include <cstdint>
#include <vector>

#include <silkworm/core/execution/evm.hpp>
#include <silkworm/core/protocol/rule_set.hpp>
#include <silkworm/core/state/state.hpp>
#include <silkworm/core/types/block.hpp>
#include <silkworm/core/types/receipt.hpp>
#include <silkworm/core/types/transaction.hpp>

namespace silkworm {

class ExecutionProcessor {
  public:
    ExecutionProcessor(const ExecutionProcessor&) = delete;
    ExecutionProcessor& operator=(const ExecutionProcessor&) = delete;

    ExecutionProcessor(const Block& block, protocol::RuleSet& rule_set, State& state, const ChainConfig& config);

    /**
     * Execute a transaction, but do not write to the DB yet.
     * Precondition: transaction must be valid.
     */
    void execute_transaction(const Transaction& txn, Receipt& receipt) noexcept;

    //! \brief Execute the block.
    //! \remarks Warning: This method does not verify state root; pre-Byzantium receipt root isn't validated either.
    //! \pre RuleSet's validate_block_header & pre_validate_block_body must return kOk.
    [[nodiscard]] ValidationResult execute_block(std::vector<Receipt>& receipts) noexcept;

    //! \brief Flush IntraBlockState into cumulative State.
    void flush_state();

    uint64_t available_gas() const noexcept;

    EVM& evm() noexcept { return evm_; }
    const EVM& evm() const noexcept { return evm_; }

  private:
    /**
     * Execute the block, but do not write to the DB yet.
     * Does not perform any post-execution validation (for example, receipt root is not checked).
     * Precondition: validate_block_header & pre_validate_block_body must return kOk.
     */
    [[nodiscard]] ValidationResult execute_block_no_post_validation(std::vector<Receipt>& receipts) noexcept;

    //! \brief Notify the registered tracers at the start of block execution.
    void notify_block_execution_start(const Block& block);

    //! \brief Notify the registered tracers at the end of block execution.
    void notify_block_execution_end(const Block& block);

    uint64_t refund_gas(const Transaction& txn, uint64_t gas_left, uint64_t gas_refund) noexcept;

    uint64_t cumulative_gas_used_{0};
    IntraBlockState state_;
    protocol::RuleSet& rule_set_;
    EVM evm_;
};

}  // namespace silkworm
