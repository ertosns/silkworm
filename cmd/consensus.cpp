/*
   Copyright 2020-2021 The Silkworm Authors

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

#include <atomic>
#include <filesystem>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <CLI/CLI.hpp>
#include <evmc/loader.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>

#include <silkworm/chain/difficulty.hpp>
#include <silkworm/common/as_range.hpp>
#include <silkworm/common/cast.hpp>
#include <silkworm/common/endian.hpp>
#include <silkworm/common/rlp_err.hpp>
#include <silkworm/common/stopwatch.hpp>
#include <silkworm/common/terminal.hpp>
#include <silkworm/common/test_util.hpp>
#include <silkworm/concurrency/thread_pool.hpp>
#include <silkworm/concurrency/thread_safe_state_pool.hpp>
#include <silkworm/consensus/blockchain.hpp>
#include <silkworm/state/in_memory_state.hpp>

// See https://ethereum-tests.readthedocs.io

using namespace silkworm;
using namespace silkworm::consensus;

namespace fs = std::filesystem;

static const fs::path kDifficultyDir{"BasicTests"};

static const fs::path kBlockchainDir{"BlockchainTests"};

static const fs::path kTransactionDir{"TransactionTests"};

static const std::vector<fs::path> kSlowTests{
    kBlockchainDir / "GeneralStateTests" / "stTimeConsuming",
    kBlockchainDir / "GeneralStateTests" / "VMTests" / "vmPerformance",
};

static const std::vector<fs::path> kFailingTests{
    // Nonce >= 2^64 is not supported.
    // Geth excludes this test as well:
    // https://github.com/ethereum/go-ethereum/blob/v1.9.25/tests/transaction_test.go#L40
    kTransactionDir / "ttNonce" / "TransactionWithHighNonce256.json",

    // Gas limit >= 2^64 is not supported; see EIP-1985.
    // Geth excludes this test as well:
    // https://github.com/ethereum/go-ethereum/blob/v1.9.25/tests/transaction_test.go#L31
    kTransactionDir / "ttGasLimit" / "TransactionWithGasLimitxPriceOverflow.json",
};

static constexpr size_t kColumnWidth{80};

static const std::map<std::string, ChainConfig> kNetworkConfig{
    {"Frontier",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
     }},
    {"Homestead",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
         },
     }},
    {"EIP150",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
         },
     }},
    {"EIP158",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
         },
     }},
    {"Byzantium",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
         },
     }},
    {"Constantinople",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
         },
     }},
    {"ConstantinopleFix",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
             0,  // petersburg_block
         },
     }},
    {"Istanbul",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
             0,  // petersburg_block
             0,  // istanbul_block
         },
     }},
    {"Berlin",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
             0,  // petersburg_block
             0,  // istanbul_block
             0,  // berlin_block
         },
     }},
    {"London", test::kLondonConfig},
    {"FrontierToHomesteadAt5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             5,  // homestead_block
         },
     }},
    {"HomesteadToEIP150At5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             5,  // tangerine_whistle_block
         },
     }},
    {"HomesteadToDaoAt5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
         },
         5,  // dao_block
     }},
    {"EIP158ToByzantiumAt5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             5,  // byzantium_block
         },
     }},
    {"ByzantiumToConstantinopleFixAt5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             5,  // constantinople_block
             5,  // petersburg_block
         },
     }},
    {"BerlinToLondonAt5",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
             0,  // petersburg_block
             0,  // istanbul_block
             0,  // berlin_block
             5,  // london_block
         },
     }},
    {"EIP2384",
     {
         1,  // chain_id
         SealEngineType::kNoProof,
         {
             0,  // homestead_block
             0,  // tangerine_whistle_block
             0,  // spurious_dragon_block
             0,  // byzantium_block
             0,  // constantinople_block
             0,  // petersburg_block
             0,  // istanbul_block
         },
         0,  // dao_block
         0,  // muir_glacier_block
     }},
};

static const std::map<std::string, ChainConfig> kDifficultyConfig{
    {"difficulty.json", kMainnetConfig},
    {"difficultyByzantium.json", kNetworkConfig.at("Byzantium")},
    {"difficultyConstantinople.json", kNetworkConfig.at("Constantinople")},
    {"difficultyCustomMainNetwork.json", kMainnetConfig},
    {"difficultyEIP2384_random_to20M.json", kNetworkConfig.at("EIP2384")},
    {"difficultyEIP2384_random.json", kNetworkConfig.at("EIP2384")},
    {"difficultyEIP2384.json", kNetworkConfig.at("EIP2384")},
    {"difficultyFrontier.json", kNetworkConfig.at("Frontier")},
    {"difficultyHomestead.json", kNetworkConfig.at("Homestead")},
    {"difficultyMainNetwork.json", kMainnetConfig},
    {"difficultyRopsten.json", kRopstenConfig},
};

ThreadSafeExecutionStatePool execution_state_pool;
evmc_vm* exo_evm{nullptr};

// https://ethereum-tests.readthedocs.io/en/latest/test_types/blockchain_tests.html#pre-prestate-section
void init_pre_state(const nlohmann::json& pre, State& state) {
    for (const auto& entry : pre.items()) {
        const evmc::address address{to_evmc_address(from_hex(entry.key()).value())};
        const nlohmann::json& j{entry.value()};

        Account account;
        const Bytes balance_str{from_hex(j["balance"].get<std::string>()).value()};
        const auto balance{endian::from_big_compact<intx::uint256>(balance_str, /*allow_leading_zeros=*/true)};
        account.balance = *balance;
        const Bytes nonce_str{from_hex(j["nonce"].get<std::string>()).value()};
        const auto nonce{endian::from_big_compact<uint64_t>(nonce_str, /*allow_leading_zeros=*/true)};
        account.nonce = *nonce;

        const Bytes code{from_hex(j["code"].get<std::string>()).value()};
        if (!code.empty()) {
            account.incarnation = kDefaultIncarnation;
            account.code_hash = bit_cast<evmc_bytes32>(keccak256(code));
            state.update_account_code(address, account.incarnation, account.code_hash, code);
        }

        state.update_account(address, /*initial=*/std::nullopt, account);

        for (const auto& storage : j["storage"].items()) {
            Bytes key{from_hex(storage.key()).value()};
            Bytes value{from_hex(storage.value().get<std::string>()).value()};
            state.update_storage(address, account.incarnation, to_bytes32(key), /*initial=*/{}, to_bytes32(value));
        }
    }
}

enum class Status { kPassed, kFailed, kSkipped };

Status run_block(const nlohmann::json& json_block, Blockchain& blockchain) {
    bool invalid{json_block.contains("expectException")};

    std::optional<Bytes> rlp{from_hex(json_block["rlp"].get<std::string>())};
    if (!rlp) {
        if (invalid) {
            return Status::kPassed;
        }
        std::cout << "Failure to read hex" << std::endl;
        return Status::kFailed;
    }

    Block block;
    ByteView view{*rlp};
    if (rlp::decode(view, block) != rlp::DecodingResult::kOk || !view.empty()) {
        if (invalid) {
            return Status::kPassed;
        }
        std::cout << "Failure to decode RLP" << std::endl;
        return Status::kFailed;
    }

    bool check_state_root{invalid && json_block["expectException"].get<std::string>() == "InvalidStateRoot"};

    if (ValidationResult err{blockchain.insert_block(block, check_state_root)}; err != ValidationResult::kOk) {
        if (invalid) {
            return Status::kPassed;
        }
        std::cout << "Validation error " << magic_enum::enum_name<ValidationResult>(err) << std::endl;
        return Status::kFailed;
    }

    if (invalid) {
        std::cout << "Invalid block executed successfully\n";
        std::cout << "Expected: " << json_block["expectException"] << std::endl;
        return Status::kFailed;
    }

    return Status::kPassed;
}

bool post_check(const InMemoryState& state, const nlohmann::json& expected) {
    if (state.number_of_accounts() != expected.size()) {
        std::cout << "Account number mismatch: " << state.number_of_accounts() << " != " << expected.size()
                  << std::endl;
        return false;
    }

    for (const auto& entry : expected.items()) {
        const evmc::address address{to_evmc_address(from_hex(entry.key()).value())};
        const nlohmann::json& j{entry.value()};

        std::optional<Account> account{state.read_account(address)};
        if (!account) {
            std::cout << "Missing account " << entry.key() << std::endl;
            return false;
        }

        const Bytes balance_str{from_hex(j["balance"].get<std::string>()).value()};
        const auto expected_balance{endian::from_big_compact<intx::uint256>(balance_str, /*allow_leading_zeros=*/true)};
        if (account->balance != expected_balance) {
            std::cout << "Balance mismatch for " << entry.key() << ":\n"
                      << to_string(account->balance, 16) << " != " << j["balance"] << std::endl;
            return false;
        }

        const Bytes nonce_str{from_hex(j["nonce"].get<std::string>()).value()};
        const auto expected_nonce{endian::from_big_compact<uint64_t>(nonce_str, /*allow_leading_zeros=*/true)};
        if (account->nonce != expected_nonce) {
            std::cout << "Nonce mismatch for " << entry.key() << ":\n"
                      << account->nonce << " != " << *expected_nonce << std::endl;
            return false;
        }

        auto expected_code{j["code"].get<std::string>()};
        Bytes actual_code{state.read_code(account->code_hash)};
        if (actual_code != from_hex(expected_code)) {
            std::cout << "Code mismatch for " << entry.key() << ":\n"
                      << to_hex(actual_code) << " != " << expected_code << std::endl;
            return false;
        }

        size_t storage_size{state.storage_size(address, account->incarnation)};
        if (storage_size != j["storage"].size()) {
            std::cout << "Storage size mismatch for " << entry.key() << ":\n"
                      << storage_size << " != " << j["storage"].size() << std::endl;
            return false;
        }

        for (const auto& storage : j["storage"].items()) {
            Bytes key{from_hex(storage.key()).value()};
            Bytes expected_value{from_hex(storage.value().get<std::string>()).value()};
            evmc::bytes32 actual_value{state.read_storage(address, account->incarnation, to_bytes32(key))};
            if (actual_value != to_bytes32(expected_value)) {
                std::cout << "Storage mismatch for " << entry.key() << " at " << storage.key() << ":\n"
                          << to_hex(actual_value) << " != " << to_hex(expected_value) << std::endl;
                return false;
            }
        }
    }

    return true;
}

// https://ethereum-tests.readthedocs.io/en/latest/test_types/blockchain_tests.html
Status blockchain_test(const nlohmann::json& json_test, const std::optional<ChainConfig>&) {
    Bytes genesis_rlp{from_hex(json_test["genesisRLP"].get<std::string>()).value()};
    ByteView genesis_view{genesis_rlp};
    Block genesis_block;
    rlp::success_or_throw(rlp::decode(genesis_view, genesis_block));

    InMemoryState state;
    std::string network{json_test["network"].get<std::string>()};
    ChainConfig config{kNetworkConfig.at(network)};

    auto consensus_engine{consensus::engine_factory(config)};
    if (!consensus_engine) {
        std::cout << magic_enum::enum_name<SealEngineType>(config.seal_engine) << " seal engine is not supported yet"
                  << std::endl;
        return Status::kSkipped;
    }

    init_pre_state(json_test["pre"], state);

    Blockchain blockchain{state, consensus_engine, config, genesis_block};
    blockchain.state_pool = &execution_state_pool;
    blockchain.exo_evm = exo_evm;

    for (const auto& json_block : json_test["blocks"]) {
        Status status{run_block(json_block, blockchain)};
        if (status != Status::kPassed) {
            return status;
        }
    }

    if (json_test.contains("postStateHash")) {
        evmc::bytes32 state_root{state.state_root_hash()};
        std::string expected_hex{json_test["postStateHash"].get<std::string>()};
        if (state_root != to_bytes32(from_hex(expected_hex).value())) {
            std::cout << "postStateHash mismatch:\n" << to_hex(state_root) << " != " << expected_hex << std::endl;
            return Status::kFailed;
        } else {
            return Status::kPassed;
        }
    }

    if (post_check(state, json_test["postState"])) {
        return Status::kPassed;
    } else {
        return Status::kFailed;
    }
}

static void print_test_status(std::string_view key, Status status) {
    std::cout << key << " ";
    for (size_t i{key.length() + 1}; i < kColumnWidth; ++i) {
        std::cout << '.';
    }
    switch (status) {
        case Status::kPassed:
            std::cout << "\033[0;32m  Passed\033[0m" << std::endl;
            break;
        case Status::kFailed:
            std::cout << "\033[1;31m  Failed\033[0m" << std::endl;
            break;
        case Status::kSkipped:
            std::cout << " Skipped" << std::endl;
            break;
    }
}

struct [[nodiscard]] RunResults {
    size_t passed{0};
    size_t failed{0};
    size_t skipped{0};

    void add(Status status) {
        switch (status) {
            case Status::kPassed:
                ++passed;
                break;
            case Status::kFailed:
                ++failed;
                break;
            case Status::kSkipped:
                ++skipped;
                break;
        }
    }
};

std::atomic<size_t> total_passed{0};
std::atomic<size_t> total_failed{0};
std::atomic<size_t> total_skipped{0};

void run_test_file(const fs::path& file_path,
                   Status (*runner)(const nlohmann::json&, const std::optional<ChainConfig>&),
                   const std::optional<ChainConfig>& config = std::nullopt) {
    std::ifstream in{file_path.string()};
    nlohmann::json json;

    try {
        in >> json;
    } catch (nlohmann::detail::parse_error& e) {
        std::cerr << e.what() << "\n";
        print_test_status(file_path.string(), Status::kSkipped);
        ++total_skipped;
        return;
    }

    RunResults res{};

    for (const auto& test : json.items()) {
        Status status{runner(test.value(), config)};
        res.add(status);
        if (status != Status::kPassed) {
            print_test_status(test.key(), status);
        }
    }

    total_passed += res.passed;
    total_failed += res.failed;
    total_skipped += res.skipped;
}

// https://ethereum-tests.readthedocs.io/en/latest/test_types/transaction_tests.html
Status transaction_test(const nlohmann::json& j, const std::optional<ChainConfig>&) {
    Transaction txn;
    bool decoded{false};

    std::optional<Bytes> rlp{from_hex(j["txbytes"].get<std::string>())};
    if (rlp) {
        ByteView view{*rlp};
        if (rlp::decode(view, txn) == rlp::DecodingResult::kOk) {
            decoded = view.empty();
        }
    }

    for (const auto& entry : j["result"].items()) {
        const bool should_be_valid{!entry.value().contains("exception")};

        if (!decoded) {
            if (should_be_valid) {
                std::cout << "Failed to decode valid transaction" << std::endl;
                return Status::kFailed;
            } else {
                continue;
            }
        }

        ChainConfig config{kNetworkConfig.at(entry.key())};

        /* pre_validate_transaction checks for invalid signature only if from is empty, which means sender recovery
         * phase (which btw also verifies signature) was not triggered yet. In the context of tests, instead, from is
         * already valued from the json rlp payload: this makes pre_validate_transaction to incorrectly skip the
         * validation signature. Hence, we reset from to nullopt to allow proper validation flow. In any case, sender
         * recovery would be performed anyway immediately after this block.
         */
        txn.from.reset();

        if (ValidationResult err{
                pre_validate_transaction(txn, /*block_number=*/0, config, /*base_fee_per_gas=*/std::nullopt)};
            err != ValidationResult::kOk) {
            if (should_be_valid) {
                std::cout << "Validation error " << magic_enum::enum_name<ValidationResult>(err) << std::endl;
                return Status::kFailed;
            } else {
                continue;
            }
        }

        txn.recover_sender();
        if (should_be_valid && !txn.from.has_value()) {
            std::cout << "Failed to recover sender" << std::endl;
            return Status::kFailed;
        }

        if (!should_be_valid && txn.from.has_value()) {
            std::cout << entry.key() << "\n"
                      << "Sender recovered for invalid transaction" << std::endl;
            return Status::kFailed;
        }

        if (!should_be_valid) {
            continue;
        }

        const std::string expected_sender{entry.value()["sender"].get<std::string>()};
        if (txn.from != to_evmc_address(*from_hex(expected_sender))) {
            std::cout << "Sender mismatch for " << entry.key() << ":\n"
                      << to_hex(*txn.from) << " != " << expected_sender << std::endl;
            return Status::kFailed;
        }
    }

    return Status::kPassed;
}

// https://ethereum-tests.readthedocs.io/en/latest/test_types/difficulty_tests.html
Status difficulty_test(const nlohmann::json& j, const std::optional<ChainConfig>& config) {
    auto parent_timestamp{std::stoull(j["parentTimestamp"].get<std::string>(), nullptr, 0)};
    auto parent_difficulty{intx::from_string<intx::uint256>(j["parentDifficulty"].get<std::string>())};
    auto current_timestamp{std::stoull(j["currentTimestamp"].get<std::string>(), nullptr, 0)};
    auto block_number{std::stoull(j["currentBlockNumber"].get<std::string>(), nullptr, 0)};
    auto current_difficulty{intx::from_string<intx::uint256>(j["currentDifficulty"].get<std::string>())};

    bool parent_has_uncles{false};
    if (j.contains("parentUncles")) {
        auto parent_uncles{j["parentUncles"].get<std::string>()};
        parent_has_uncles = from_hex(parent_uncles).value() != full_view(kEmptyListHash);
    }

    intx::uint256 calculated_difficulty{canonical_difficulty(block_number, current_timestamp, parent_difficulty,
                                                             parent_timestamp, parent_has_uncles, *config)};
    if (calculated_difficulty == current_difficulty) {
        return Status::kPassed;
    } else {
        std::cout << "Difficulty mismatch for block " << block_number << "\n"
                  << hex(calculated_difficulty) << " != " << hex(current_difficulty) << std::endl;
        return Status::kFailed;
    }
}

bool exclude_test(const fs::path& p, const fs::path& root_dir, bool include_slow_tests) {
    const auto path_fits = [&p, &root_dir](const fs::path& e) { return root_dir / e == p; };
    return as_range::any_of(kFailingTests, path_fits) ||
           (!include_slow_tests && as_range::any_of(kSlowTests, path_fits));
}

int main(int argc, char* argv[]) {
    StopWatch sw;
    sw.start();

    CLI::App app{"Run Ethereum consensus tests"};

    std::string evm_path{};
    app.add_option("--evm", evm_path, "Path to EVMC-compliant VM");
    std::string tests_path{SILKWORM_CONSENSUS_TEST_DIR};
    app.add_option("--tests", tests_path, "Path to consensus tests", /*defaulted=*/true)->check(CLI::ExistingDirectory);
    unsigned num_threads{std::thread::hardware_concurrency()};
    app.add_option("--threads", num_threads, "Number of parallel threads", /*defaulted=*/true);
    bool include_slow_tests{false};
    app.add_flag("--slow", include_slow_tests, "Run slow tests");

    CLI11_PARSE(app, argc, argv);
    init_terminal();

    if (!evm_path.empty()) {
        evmc_loader_error_code err;
        exo_evm = evmc_load_and_configure(evm_path.c_str(), &err);
        if (err) {
            std::cerr << "Failed to load EVM: " << evmc_last_error_msg() << std::endl;
            return -1;
        }
    }

    thread_pool thread_pool{num_threads, /*stack_size=*/16 * kMebi};

    const fs::path root_dir{tests_path};

    for (const auto& entry : kDifficultyConfig) {
        const fs::path path{root_dir / kDifficultyDir / entry.first};
        const ChainConfig config{entry.second};
        thread_pool.push_task([path, config]() { run_test_file(path, difficulty_test, config); });
    }

    for (auto i = fs::recursive_directory_iterator(root_dir / kBlockchainDir); i != fs::recursive_directory_iterator{};
         ++i) {
        if (exclude_test(*i, root_dir, include_slow_tests)) {
            ++total_skipped;
            i.disable_recursion_pending();
        } else if (fs::is_regular_file(i->path())) {
            const fs::path path{*i};
            thread_pool.push_task([path]() { run_test_file(path, blockchain_test); });
        }
    }

    for (auto i = fs::recursive_directory_iterator(root_dir / kTransactionDir); i != fs::recursive_directory_iterator{};
         ++i) {
        if (exclude_test(*i, root_dir, include_slow_tests)) {
            ++total_skipped;
            i.disable_recursion_pending();
        } else if (fs::is_regular_file(i->path())) {
            const fs::path path{*i};
            thread_pool.push_task([path]() { run_test_file(path, transaction_test); });
        }
    }

    thread_pool.wait_for_tasks();

    std::cout << kColorGreen << total_passed << " tests passed" << kColorReset << ", ";
    if (total_failed != 0) {
        std::cout << kColorMaroonHigh;
    }
    std::cout << total_failed << " failed";
    if (total_failed != 0) {
        std::cout << kColorReset;
    }
    std::cout << ", " << total_skipped << " skipped";

    const auto [_, duration] = sw.lap();
    std::cout << " in " << StopWatch::format(duration) << std::endl;

    return total_failed != 0;
}
