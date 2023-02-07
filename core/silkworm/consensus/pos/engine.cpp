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

#include "engine.hpp"

#include <silkworm/common/assert.hpp>

namespace silkworm::consensus {

ValidationResult ProofOfStakeEngine::validate_seal(const BlockHeader& header) {
    return header.nonce == BlockHeader::NonceType{} ? ValidationResult::kOk : ValidationResult::kInvalidNonce;
}

ValidationResult ProofOfStakeEngine::validate_difficulty(const BlockHeader& header, const BlockHeader&) {
    return header.difficulty == 0 ? ValidationResult::kOk : ValidationResult::kWrongDifficulty;
}

void ProofOfStakeEngine::finalize(IntraBlockState& state, const Block& block, evmc_revision revision) {
    if (revision < EVMC_SHANGHAI) {
        return;
    }

    SILKWORM_ASSERT(block.withdrawals);

    // See EIP-4895: Beacon chain push withdrawals as operations
    for (const Withdrawal& w : *block.withdrawals) {
        const auto amount_in_wei{intx::uint256{w.amount} * intx::uint256{kGiga}};
        state.add_to_balance(w.address, amount_in_wei);
    }
}

}  // namespace silkworm::consensus
