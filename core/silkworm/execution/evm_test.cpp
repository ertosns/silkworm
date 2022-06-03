/*
   Copyright 2020-2022 The Silkworm Authors

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

#include "evm.hpp"

#include <map>
#include <vector>

#include <catch2/catch.hpp>
#include <evmone/execution_state.hpp>

#include <silkworm/chain/protocol_param.hpp>
#include <silkworm/common/test_util.hpp>
#include <silkworm/common/util.hpp>
#include <silkworm/execution/precompiled.hpp>
#include <silkworm/state/in_memory_state.hpp>

#include "address.hpp"

namespace silkworm {

TEST_CASE("Value transfer") {
    Block block{};
    block.header.number = 10336006;

    evmc::address from{0x0a6bb546b9208cfab9e8fa2b9b2c042b18df7030_address};
    evmc::address to{0x8b299e2b7d7f43c0ce3068263545309ff4ffb521_address};
    intx::uint256 value{10'200'000'000'000'000};

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, kMainnetConfig};

    CHECK(state.get_balance(from) == 0);
    CHECK(state.get_balance(to) == 0);

    Transaction txn{};
    txn.from = from;
    txn.to = to;
    txn.value = value;

    CallResult res{evm.execute(txn, 0)};
    CHECK(res.status == EVMC_INSUFFICIENT_BALANCE);
    CHECK(res.data.empty());

    state.add_to_balance(from, kEther);

    res = evm.execute(txn, 0);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    CHECK(state.get_balance(from) == kEther - value);
    CHECK(state.get_balance(to) == value);
    CHECK(state.touched().count(from) == 1);
    CHECK(state.touched().count(to) == 1);
}

TEST_CASE("Smart contract with storage") {
    Block block{};
    block.header.number = 1;
    evmc::address caller{0x0a6bb546b9208cfab9e8fa2b9b2c042b18df7030_address};

    // This contract initially sets its 0th storage to 0x2a
    // and its 1st storage to 0x01c9.
    // When called, it updates the 0th storage to the input provided.
    Bytes code{*from_hex("602a5f556101c960015560048060135f395ff35f355f55")};
    // https://github.com/CoinCulture/evm-tools/blob/master/analysis/guide.md#contracts
    // 0x00     PUSH1  => 2a
    // 0x02     PUSH0
    // 0x03     SSTORE         // storage[0] = 0x2a
    // 0x04     PUSH2  => 01c9
    // 0x07     PUSH1  => 01
    // 0x09     SSTORE         // storage[1] = 0x01c9
    // 0x0a     PUSH1  => 04   // deploy begin
    // 0x0c     DUP1
    // 0x0d     PUSH1  => 13
    // 0x0f     PUSH0
    // 0x10     CODECOPY
    // 0x11     PUSH0
    // 0x12     RETURN         // deploy end
    // 0x13     PUSH0          // contract code
    // 0x14     CALLDATALOAD
    // 0x15     PUSH0
    // 0x16     SSTORE         // storage[0] = input[0]

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, test::kShanghaiConfig};

    Transaction txn{};
    txn.from = caller;
    txn.data = code;

    uint64_t gas{0};
    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_OUT_OF_GAS);
    CHECK(res.data.empty());

    gas = 50'000;
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(to_hex(res.data) == "5f355f55");

    evmc::address contract_address{create_address(caller, /*nonce=*/1)};
    evmc::bytes32 key0{};
    CHECK(to_hex(zeroless_view(state.get_current_storage(contract_address, key0))) == "2a");

    evmc::bytes32 new_val{to_bytes32(*from_hex("f5"))};
    txn.to = contract_address;
    txn.data = ByteView{new_val};

    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());
    CHECK(state.get_current_storage(contract_address, key0) == new_val);
}

TEST_CASE("Maximum call depth") {
    Block block{};
    block.header.number = 1'431'916;
    evmc::address caller{0x8e4d1ea201b908ab5e1f5a1c3f9f1b4f6c1e9cf1_address};
    evmc::address contract{0x3589d05a1ec4af9f65b0e5554e645707775ee43c_address};

    // The contract just calls itself recursively a given number of times.
    Bytes code{*from_hex("60003580600857005b6001900360005260008060208180305a6103009003f1602357fe5b")};
    /* https://github.com/CoinCulture/evm-tools
    0      PUSH1  => 00
    2      CALLDATALOAD
    3      DUP1
    4      PUSH1  => 08
    6      JUMPI
    7      STOP
    8      JUMPDEST
    9      PUSH1  => 01
    11     SWAP1
    12     SUB
    13     PUSH1  => 00
    15     MSTORE
    16     PUSH1  => 00
    18     DUP1
    19     PUSH1  => 20
    21     DUP2
    22     DUP1
    23     ADDRESS
    24     GAS
    25     PUSH2  => 0300
    28     SWAP1
    29     SUB
    30     CALL
    31     PUSH1  => 23
    33     JUMPI
    34     INVALID
    35     JUMPDEST
    */

    InMemoryState db;
    IntraBlockState state{db};
    state.set_code(contract, code);

    EVM evm{block, state, kMainnetConfig};

    AdvancedAnalysisCache analysis_cache{/*maxSize=*/16};
    evm.advanced_analysis_cache = &analysis_cache;

    Transaction txn{};
    txn.from = caller;
    txn.to = contract;

    uint64_t gas{1'000'000};
    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    evmc::bytes32 num_of_recursions{to_bytes32(*from_hex("0400"))};
    txn.data = ByteView{num_of_recursions};
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    num_of_recursions = to_bytes32(*from_hex("0401"));
    txn.data = ByteView{num_of_recursions};
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_INVALID_INSTRUCTION);
    CHECK(res.data.empty());
}

TEST_CASE("DELEGATECALL") {
    Block block{};
    block.header.number = 1'639'560;
    evmc::address caller_address{0x8e4d1ea201b908ab5e1f5a1c3f9f1b4f6c1e9cf1_address};
    evmc::address callee_address{0x3589d05a1ec4af9f65b0e5554e645707775ee43c_address};

    // The callee writes the ADDRESS to storage.
    Bytes callee_code{*from_hex("30600055")};
    /* https://github.com/CoinCulture/evm-tools
    0      ADDRESS
    1      PUSH1  => 00
    3      SSTORE
    */

    // The caller delegate-calls the input contract.
    Bytes caller_code{*from_hex("6000808080803561eeeef4")};
    /* https://github.com/CoinCulture/evm-tools
    0      PUSH1  => 00
    2      DUP1
    3      DUP1
    4      DUP1
    5      DUP1
    6      CALLDATALOAD
    7      PUSH2  => eeee
    10     DELEGATECALL
    */

    InMemoryState db;
    IntraBlockState state{db};
    state.set_code(caller_address, caller_code);
    state.set_code(callee_address, callee_code);

    EVM evm{block, state, kMainnetConfig};

    Transaction txn{};
    txn.from = caller_address;
    txn.to = caller_address;
    txn.data = ByteView{to_bytes32(callee_address)};

    uint64_t gas{1'000'000};
    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    evmc::bytes32 key0{};
    CHECK(to_hex(zeroless_view(state.get_current_storage(caller_address, key0))) == to_hex(caller_address));
}

// https://eips.ethereum.org/EIPS/eip-211#specification
TEST_CASE("CREATE should only return on failure") {
    Block block{};
    block.header.number = 4'575'910;
    evmc::address caller{0xf466859ead1932d743d622cb74fc058882e8648a_address};

    Bytes code{
        *from_hex("0x602180601360003960006000f0503d600055006211223360005260206000602060006000600461900"
                  "0f1503d60005560206000f3")};
    /* https://github.com/CoinCulture/evm-tools
    0      PUSH1  => 21
    2      DUP1
    3      PUSH1  => 13
    5      PUSH1  => 00
    7      CODECOPY
    8      PUSH1  => 00
    10     PUSH1  => 00
    12     CREATE
    13     POP
    14     RETURNDATASIZE
    15     PUSH1  => 00
    17     SSTORE
    18     STOP
    19     PUSH3  => 112233
    23     PUSH1  => 00
    25     MSTORE
    26     PUSH1  => 20
    28     PUSH1  => 00
    30     PUSH1  => 20
    32     PUSH1  => 00
    34     PUSH1  => 00
    36     PUSH1  => 04
    38     PUSH2  => 9000
    41     CALL
    42     POP
    43     RETURNDATASIZE
    44     PUSH1  => 00
    46     SSTORE
    47     PUSH1  => 20
    49     PUSH1  => 00
    51     RETURN
    */

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, kMainnetConfig};

    Transaction txn{};
    txn.from = caller;
    txn.data = code;

    uint64_t gas{150'000};
    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    evmc::address contract_address{create_address(caller, /*nonce=*/0)};
    evmc::bytes32 key0{};
    CHECK(is_zero(state.get_current_storage(contract_address, key0)));
}

// https://github.com/ethereum/EIPs/issues/684
TEST_CASE("Contract overwrite") {
    Block block{};
    block.header.number = 7'753'545;

    Bytes old_code{*from_hex("6000")};
    Bytes new_code{*from_hex("6001")};

    evmc::address caller{0x92a1d964b8fc79c5694343cc943c27a94a3be131_address};

    evmc::address contract_address{create_address(caller, /*nonce=*/0)};

    InMemoryState db;
    IntraBlockState state{db};
    state.set_code(contract_address, old_code);

    EVM evm{block, state, kMainnetConfig};

    Transaction txn{};
    txn.from = caller;
    txn.data = new_code;

    uint64_t gas{100'000};
    CallResult res{evm.execute(txn, gas)};

    CHECK(res.status == EVMC_INVALID_INSTRUCTION);
    CHECK(res.gas_left == 0);
    CHECK(res.data.empty());
}

TEST_CASE("EIP-3541: Reject new contracts starting with the 0xEF byte") {
    ChainConfig config{kMainnetConfig};
    config.set_revision_block(EVMC_LONDON, 13'000'000);

    Block block;
    block.header.number = 13'500'000;

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, config};

    Transaction txn;
    txn.from = 0x1000000000000000000000000000000000000000_address;
    const uint64_t gas{50'000};

    // https://eips.ethereum.org/EIPS/eip-3541#test-cases
    txn.data = *from_hex("0x60ef60005360016000f3");
    CHECK(evm.execute(txn, gas).status == EVMC_CONTRACT_VALIDATION_FAILURE);

    txn.data = *from_hex("0x60ef60005360026000f3");
    CHECK(evm.execute(txn, gas).status == EVMC_CONTRACT_VALIDATION_FAILURE);

    txn.data = *from_hex("0x60ef60005360036000f3");
    CHECK(evm.execute(txn, gas).status == EVMC_CONTRACT_VALIDATION_FAILURE);

    txn.data = *from_hex("0x60ef60005360206000f3");
    CHECK(evm.execute(txn, gas).status == EVMC_CONTRACT_VALIDATION_FAILURE);

    txn.data = *from_hex("0x60fe60005360016000f3");
    CHECK(evm.execute(txn, gas).status == EVMC_SUCCESS);
}

class TestTracer : public EvmTracer {
  public:
    TestTracer(std::optional<evmc::address> contract_address = std::nullopt,
                std::optional<evmc::bytes32> key = std::nullopt)
        : contract_address_(contract_address), key_(key) {}

    void on_execution_start(evmc_revision rev, const evmc_message& msg,
                            evmone::bytes_view bytecode) noexcept override {
        execution_start_called_ = true;
        rev_ = rev;
        msg_ = msg;
        bytecode_ = Bytes{bytecode};
    }
    void on_instruction_start(uint32_t pc, const intx::uint256* /*stack_top*/, int /*stack_height*/, 
        const evmone::ExecutionState& state, const IntraBlockState& intra_block_state) noexcept override {
        pc_stack_.push_back(pc);
        memory_size_stack_[pc] = state.memory.size();
        if (contract_address_) {
            storage_stack_[pc] =
                intra_block_state.get_current_storage(contract_address_.value(), key_.value_or(evmc::bytes32{}));
        }
    }
    void on_execution_end(const evmc_result& res, const IntraBlockState& intra_block_state) noexcept override {
        execution_end_called_ = true;
        result_ = {res.status_code, static_cast<uint64_t>(res.gas_left), {res.output_data, res.output_size}};
        if (contract_address_ && pc_stack_.size() > 0) {
            const auto pc = pc_stack_.back();
            storage_stack_[pc] =
                intra_block_state.get_current_storage(contract_address_.value(), key_.value_or(evmc::bytes32{}));
        }
    }
    void on_precompiled_run(const evmc::result& /*result*/, int64_t /*gas*/,
        const IntraBlockState& /*intra_block_state*/) noexcept override {
    }
    void on_reward_granted(const CallResult& /*result*/,
        const IntraBlockState& /*intra_block_state*/) noexcept override {
    }

    bool execution_start_called() const { return execution_start_called_; }
    bool execution_end_called() const { return execution_end_called_; }
    const Bytes& bytecode() const { return bytecode_; }
    const evmc_revision& rev() const { return rev_; }
    const evmc_message& msg() const { return msg_; }
    const std::vector<uint32_t>& pc_stack() const { return pc_stack_; }
    const std::map<uint32_t, std::size_t>& memory_size_stack() const { return memory_size_stack_; }
    const std::map<uint32_t, evmc::bytes32>& storage_stack() const { return storage_stack_; }
    const CallResult& result() const { return result_; }

  private:
    bool execution_start_called_{false};
    bool execution_end_called_{false};
    std::optional<evmc::address> contract_address_;
    std::optional<evmc::bytes32> key_;
    evmc_revision rev_;
    evmc_message msg_;
    Bytes bytecode_;
    std::vector<uint32_t> pc_stack_;
    std::map<uint32_t, std::size_t> memory_size_stack_;
    std::map<uint32_t, evmc::bytes32> storage_stack_;
    CallResult result_;
};

TEST_CASE("Tracing smart contract with storage") {
    Block block{};
    block.header.number = 10'336'006;
    evmc::address caller{0x0a6bb546b9208cfab9e8fa2b9b2c042b18df7030_address};

    // This contract initially sets its 0th storage to 0x2a
    // and its 1st storage to 0x01c9.
    // When called, it updates the 0th storage to the input provided.
    Bytes code{*from_hex("602a6000556101c960015560068060166000396000f3600035600055")};
    // https://github.com/CoinCulture/evm-tools
    // 0      PUSH1  => 2a
    // 2      PUSH1  => 00
    // 4      SSTORE         // storage[0] = 0x2a
    // 5      PUSH2  => 01c9
    // 8      PUSH1  => 01
    // 10     SSTORE         // storage[1] = 0x01c9
    // 11     PUSH1  => 06   // deploy begin
    // 13     DUP1
    // 14     PUSH1  => 16
    // 16     PUSH1  => 00
    // 18     CODECOPY
    // 19     PUSH1  => 00
    // 21     RETURN         // deploy end
    // 22     PUSH1  => 00   // contract code
    // 24     CALLDATALOAD
    // 25     PUSH1  => 00
    // 27     SSTORE         // storage[0] = input[0]

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, kMainnetConfig};

    Transaction txn{};
    txn.from = caller;
    txn.data = code;

    CHECK(evm.tracers().empty());

    // First execution: out of gas
    TestTracer tracer1;
    evm.add_tracer(tracer1);
    CHECK(evm.tracers().size() == 1);

    uint64_t gas{0};
    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_OUT_OF_GAS);
    CHECK(res.data.empty());

    CHECK((tracer1.execution_start_called() && tracer1.execution_end_called()));
    CHECK(tracer1.rev() == evmc_revision::EVMC_ISTANBUL);
    CHECK(tracer1.msg().kind == evmc_call_kind::EVMC_CALL);
    CHECK(tracer1.msg().flags == 0);
    CHECK(tracer1.msg().depth == 0);
    CHECK(tracer1.msg().gas == 0);
    CHECK(tracer1.bytecode() == code);
    CHECK(tracer1.pc_stack() == std::vector<uint32_t>{0});
    CHECK(tracer1.memory_size_stack() == std::map<uint32_t, std::size_t>{{0, 0}});
    CHECK(tracer1.result().status == EVMC_OUT_OF_GAS);
    CHECK(tracer1.result().gas_left == 0);
    CHECK(tracer1.result().data == Bytes{});

    // Second execution: success
    TestTracer tracer2;
    evm.add_tracer(tracer2);
    CHECK(evm.tracers().size() == 2);

    gas = 50'000;
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data == from_hex("600035600055"));

    CHECK((tracer2.execution_start_called() && tracer2.execution_end_called()));
    CHECK(tracer2.rev() == evmc_revision::EVMC_ISTANBUL);
    CHECK(tracer2.msg().kind == evmc_call_kind::EVMC_CALL);
    CHECK(tracer2.msg().flags == 0);
    CHECK(tracer2.msg().depth == 0);
    CHECK(tracer2.msg().gas == 50'000);
    CHECK(tracer2.bytecode() == code);
    CHECK(tracer2.pc_stack() == std::vector<uint32_t>{0, 2, 4, 5, 8, 10, 11, 13, 14, 16, 18, 19, 21});
    CHECK(tracer2.memory_size_stack() == std::map<uint32_t, std::size_t>{{0, 0},
                                                                         {2, 0},
                                                                         {4, 0},
                                                                         {5, 0},
                                                                         {8, 0},
                                                                         {10, 0},
                                                                         {11, 0},
                                                                         {13, 0},
                                                                         {14, 0},
                                                                         {16, 0},
                                                                         {18, 0},
                                                                         {19, 32},
                                                                         {21, 32}});
    CHECK(tracer2.result().status == EVMC_SUCCESS);
    CHECK(tracer2.result().gas_left == 9964);
    CHECK(tracer2.result().data == res.data);

    // Third execution: success
    evmc::address contract_address{create_address(caller, 1)};
    evmc::bytes32 key0{};

    TestTracer tracer3{contract_address, key0};
    evm.add_tracer(tracer3);
    CHECK(evm.tracers().size() == 3);

    CHECK(to_hex(zeroless_view(state.get_current_storage(contract_address, key0))) == "2a");
    evmc::bytes32 new_val{to_bytes32(*from_hex("f5"))};
    txn.to = contract_address;
    txn.data = ByteView{new_val};
    gas = 50'000;
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());
    CHECK(state.get_current_storage(contract_address, key0) == new_val);

    CHECK((tracer3.execution_start_called() && tracer3.execution_end_called()));
    CHECK(tracer3.rev() == evmc_revision::EVMC_ISTANBUL);
    CHECK(tracer3.msg().kind == evmc_call_kind::EVMC_CALL);
    CHECK(tracer3.msg().flags == 0);
    CHECK(tracer3.msg().depth == 0);
    CHECK(tracer3.msg().gas == 50'000);
    CHECK(tracer3.storage_stack() == std::map<uint32_t, evmc::bytes32>{
                                         {0, to_bytes32(*from_hex("2a"))},
                                         {2, to_bytes32(*from_hex("2a"))},
                                         {3, to_bytes32(*from_hex("2a"))},
                                         {5, to_bytes32(*from_hex("f5"))},
                                     });
    CHECK(tracer3.pc_stack() == std::vector<uint32_t>{0, 2, 3, 5});
    CHECK(tracer3.memory_size_stack() == std::map<uint32_t, std::size_t>{{0, 0}, {2, 0}, {3, 0}, {5, 0}});
    CHECK(tracer3.result().status == EVMC_SUCCESS);
    CHECK(tracer3.result().gas_left == 49191);
    CHECK(tracer3.result().data == Bytes{});
}

TEST_CASE("Tracing smart contract w/o code") {
    Block block{};
    block.header.number = 10'336'006;

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, kMainnetConfig};
    CHECK(evm.tracers().empty());

    TestTracer tracer1;
    evm.add_tracer(tracer1);
    CHECK(evm.tracers().size() == 1);

    // Deploy contract without code
    evmc::address caller{0x0a6bb546b9208cfab9e8fa2b9b2c042b18df7030_address};
    Bytes code{};

    Transaction txn{};
    txn.from = caller;
    txn.data = code;
    uint64_t gas{50'000};

    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    CHECK(tracer1.execution_start_called());
    CHECK(tracer1.execution_end_called());
    CHECK(tracer1.rev() == evmc_revision::EVMC_ISTANBUL);
    CHECK(tracer1.bytecode() == code);
    CHECK(tracer1.pc_stack() == std::vector<uint32_t>{});
    CHECK(tracer1.memory_size_stack() == std::map<uint32_t, std::size_t>{});
    CHECK(tracer1.result().status == EVMC_SUCCESS);
    CHECK(tracer1.result().gas_left == gas);
    CHECK(tracer1.result().data == Bytes{});

    // Send message to empty contract
    evmc::address contract_address{create_address(caller, 1)};
    evmc::bytes32 key0{};

    TestTracer tracer2{contract_address, key0};
    evm.add_tracer(tracer2);
    CHECK(evm.tracers().size() == 2);

    txn.to = contract_address;
    txn.data = ByteView{to_bytes32(*from_hex("f5"))};
    res = evm.execute(txn, gas);
    CHECK(res.status == EVMC_SUCCESS);
    CHECK(res.data.empty());

    CHECK(tracer2.execution_start_called());
    CHECK(tracer2.execution_end_called());
    CHECK(tracer2.rev() == evmc_revision::EVMC_ISTANBUL);
    CHECK(tracer2.bytecode() == code);
    CHECK(tracer2.pc_stack() == std::vector<uint32_t>{});
    CHECK(tracer2.memory_size_stack() == std::map<uint32_t, std::size_t>{});
    CHECK(tracer2.result().status == EVMC_SUCCESS);
    CHECK(tracer2.result().gas_left == gas);
    CHECK(tracer2.result().data == Bytes{});
}

TEST_CASE("Tracing precompiled contract failure") {
    Block block{};
    block.header.number = 10'336'006;

    InMemoryState db;
    IntraBlockState state{db};
    EVM evm{block, state, kMainnetConfig};
    CHECK(evm.tracers().empty());

    TestTracer tracer1;
    evm.add_tracer(tracer1);
    CHECK(evm.tracers().size() == 1);

    // Execute transaction Deploy contract without code
    evmc::address caller{0x0a6bb546b9208cfab9e8fa2b9b2c042b18df7030_address};

    evmc::address max_precompiled{};
    max_precompiled.bytes[kAddressLength - 1] = precompiled::kNumOfIstanbulContracts;

    Transaction txn{};
    txn.from = caller;
    txn.to = max_precompiled;
    uint64_t gas{50'000};

    CallResult res{evm.execute(txn, gas)};
    CHECK(res.status == EVMC_PRECOMPILE_FAILURE);
}

}  // namespace silkworm
