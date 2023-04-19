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

#include <catch2/catch.hpp>

#include <silkworm/core/common/test_util.hpp>

namespace silkworm::consensus {

TEST_CASE("Consensus Engine factory") {
    std::unique_ptr<IEngine> consensus_engine;
    consensus_engine = engine_factory(kMainnetConfig);  // Ethash consensus engine
    CHECK(consensus_engine != nullptr);
    consensus_engine = engine_factory(kSepoliaConfig);  // Ethash consensus engine
    CHECK(consensus_engine != nullptr);
    consensus_engine = engine_factory(test::kLondonConfig);  // Noproof consensus engine
    CHECK(consensus_engine != nullptr);
    consensus_engine = engine_factory(kRinkebyConfig);  // Clique consensus engine
    CHECK(consensus_engine != nullptr);
    consensus_engine = engine_factory(kGoerliConfig);  // Clique consensus engine
    CHECK(consensus_engine != nullptr);
    consensus_engine = engine_factory(ChainConfig{.seal_engine = SealEngineType::kAuRA});
    CHECK(consensus_engine == nullptr);
}

TEST_CASE("Consensus Engine Seal") {
    std::unique_ptr<IEngine> consensus_engine{engine_factory(ChainConfig{.seal_engine = SealEngineType::kEthash})};
    BlockHeader fake_header{};
    CHECK(consensus_engine->validate_seal(fake_header) != ValidationResult::kOk);
    consensus_engine = engine_factory(ChainConfig{.seal_engine = SealEngineType::kNoProof});
    CHECK(consensus_engine->validate_seal(fake_header) == ValidationResult::kOk);
}

TEST_CASE("Validate transaction types") {
    const std::optional<intx::uint256> base_fee_per_gas{std::nullopt};
    const std::optional<intx::uint256> data_gas_price{std::nullopt};

    Transaction txn;
    txn.type = Transaction::Type::kLegacy;
    CHECK(pre_validate_transaction(txn, EVMC_ISTANBUL, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_BERLIN, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);

    txn.type = static_cast<Transaction::Type>(0x03);  // unsupported transaction type
    CHECK(pre_validate_transaction(txn, EVMC_ISTANBUL, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_BERLIN, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);

    txn.type = Transaction::Type::kEip2930;
    CHECK(pre_validate_transaction(txn, EVMC_ISTANBUL, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_BERLIN, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);

    txn.type = Transaction::Type::kEip1559;
    CHECK(pre_validate_transaction(txn, EVMC_ISTANBUL, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_BERLIN, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kUnsupportedTransactionType);
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kUnsupportedTransactionType);
}

TEST_CASE("Validate max_fee_per_gas") {
    const std::optional<intx::uint256> base_fee_per_gas{1'000'000'000};
    const std::optional<intx::uint256> data_gas_price{std::nullopt};

    Transaction txn;
    txn.type = Transaction::Type::kEip1559;

    txn.max_priority_fee_per_gas = 500'000'000;
    txn.max_fee_per_gas = 700'000'000;
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kMaxFeeLessThanBase);

    txn.max_priority_fee_per_gas = 3'000'000'000;
    txn.max_fee_per_gas = 2'000'000'000;
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) ==
          ValidationResult::kMaxPriorityFeeGreaterThanMax);

    txn.max_priority_fee_per_gas = 2'000'000'000;
    txn.max_fee_per_gas = 2'000'000'000;
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kMaxPriorityFeeGreaterThanMax);

    txn.max_priority_fee_per_gas = 1'000'000'000;
    txn.max_fee_per_gas = 2'000'000'000;
    CHECK(pre_validate_transaction(txn, EVMC_LONDON, 1, base_fee_per_gas, data_gas_price) !=
          ValidationResult::kMaxPriorityFeeGreaterThanMax);
}

}  // namespace silkworm::consensus
