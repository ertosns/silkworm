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

#include "transaction.hpp"

#include <catch2/catch.hpp>
#include <evmc/evmc.hpp>

namespace silkworm::rpc {

using Catch::Matchers::Message;
using evmc::literals::operator""_address, evmc::literals::operator""_bytes32;

TEST_CASE("serialize empty transaction", "[silkrpc][to_json]") {
    silkworm::Transaction txn{};
    nlohmann::json j = txn;
    CHECK(j == R"({
        "nonce":"0x0",
        "gas":"0x0",
        "to":null,
        "type":"0x0",
        "value":"0x0",
        "input":"0x",
        "hash":"0x3763e4f6e4198413383534c763f3f5dac5c5e939f0a81724e3beb96d6e2ad0d5",
        "r":"0x0",
        "s":"0x0",
        "v":"0x1b"
    })"_json);
}

TEST_CASE("serialize legacy transaction (type=0)", "[silkrpc][to_json]") {
    // https://etherscan.io/tx/0x5c504ed432cb51138bcf09aa5e8a410dd4a1e204ef84bfed1be16dfba1b22060
    // Block 46147
    silkworm::Transaction txn1{
        {.type = TransactionType::kLegacy,
         .nonce = 0,
         .max_priority_fee_per_gas = 50'000 * kGiga,
         .max_fee_per_gas = 50'000 * kGiga,
         .gas_limit = 21'000,
         .to = 0x5df9b87991262f6ba471f09758cde1c0fc1de734_address,
         .value = 31337},
        true,                                                                                                    // odd_y_parity
        intx::from_string<intx::uint256>("0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0"),  // r
        intx::from_string<intx::uint256>("0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a"),  // s
    };
    nlohmann::json j1 = txn1;
    CHECK(j1 == R"({
        "from":"0xa1e4380a3b1f749673e270229993ee55f35663b4",
        "gas":"0x5208",
        "hash":"0x5c504ed432cb51138bcf09aa5e8a410dd4a1e204ef84bfed1be16dfba1b22060",
        "input":"0x",
        "nonce":"0x0",
        "r":"0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0",
        "s":"0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a",
        "to":"0x5df9b87991262f6ba471f09758cde1c0fc1de734",
        "type":"0x0",
        "v":"0x1c",
        "value":"0x7a69"
    })"_json);

    silkworm::rpc::Transaction txn2{
        {
            {.type = TransactionType::kLegacy,
             .nonce = 0,
             .max_priority_fee_per_gas = 50'000 * kGiga,
             .max_fee_per_gas = 50'000 * kGiga,
             .gas_limit = 21'000,
             .to = 0x5df9b87991262f6ba471f09758cde1c0fc1de734_address,
             .value = 31337},
            true,                                                                                                    // odd_y_parity
            intx::from_string<intx::uint256>("0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0"),  // r
            intx::from_string<intx::uint256>("0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a"),  // s
            0x007fb8417eb9ad4d958b050fc3720d5b46a2c053_address,                                                      // from
        },
        0x4e3a3754410177e6937ef1f84bba68ea139e8d1a2258c5f85db9f1cd715a1bdd_bytes32,  // block_hash
        46147,                                                                       // block_number
        intx::uint256{0},                                                            // block_base_fee_per_gas
        0                                                                            // transactionIndex
    };
    nlohmann::json j2 = txn2;
    CHECK(j2 == R"({
        "blockHash":"0x4e3a3754410177e6937ef1f84bba68ea139e8d1a2258c5f85db9f1cd715a1bdd",
        "blockNumber":"0xb443",
        "from":"0x007fb8417eb9ad4d958b050fc3720d5b46a2c053",
        "gas":"0x5208",
        "gasPrice":"0x2d79883d2000",
        "hash":"0x5c504ed432cb51138bcf09aa5e8a410dd4a1e204ef84bfed1be16dfba1b22060",
        "input":"0x",
        "nonce":"0x0",
        "r":"0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0",
        "s":"0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a",
        "to":"0x5df9b87991262f6ba471f09758cde1c0fc1de734",
        "transactionIndex":"0x0",
        "type":"0x0",
        "v":"0x1c",
        "value":"0x7a69"
    })"_json);
    silkworm::rpc::Transaction txn3{
        {
            {.type = TransactionType::kLegacy,
             .nonce = 0,
             .max_priority_fee_per_gas = 50'000 * kGiga,
             .max_fee_per_gas = 50'000 * kGiga,
             .gas_limit = 21'000,
             .to = 0x5df9b87991262f6ba471f09758cde1c0fc1de734_address,
             .value = 31337},
            true,                                                                                                    // odd_y_parity
            intx::from_string<intx::uint256>("0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0"),  // r
            intx::from_string<intx::uint256>("0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a"),  // s
            0x007fb8417eb9ad4d958b050fc3720d5b46a2c053_address,                                                      // from
        },
        0x4e3a3754410177e6937ef1f84bba68ea139e8d1a2258c5f85db9f1cd715a1bdd_bytes32,  // block_hash
        46147,                                                                       // block_number
        intx::uint256{0},                                                            // block_base_fee_per_gas
        0,                                                                           // transactionIndex
        true                                                                         // queued_in_pool
    };
    nlohmann::json j3 = txn3;
    CHECK(j3 == R"({
        "blockHash":null,
        "blockNumber":null,
        "from":"0x007fb8417eb9ad4d958b050fc3720d5b46a2c053",
        "gas":"0x5208",
        "gasPrice":"0x2d79883d2000",
        "hash":"0x5c504ed432cb51138bcf09aa5e8a410dd4a1e204ef84bfed1be16dfba1b22060",
        "input":"0x",
        "nonce":"0x0",
        "r":"0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0",
        "s":"0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a",
        "to":"0x5df9b87991262f6ba471f09758cde1c0fc1de734",
        "transactionIndex":null,
        "type":"0x0",
        "v":"0x1c",
        "value":"0x7a69"
    })"_json);
}

TEST_CASE("serialize EIP-2930 transaction (type=1)", "[silkrpc][to_json]") {
    silkworm::Transaction txn1{
        {.type = TransactionType::kAccessList,
         .chain_id = 1,
         .nonce = 0,
         .max_priority_fee_per_gas = 20000000000,
         .max_fee_per_gas = 20000000000,
         .gas_limit = 0,
         .to = 0x0715a7794a1dc8e42615f059dd6e406a6594651a_address,
         .value = 0,
         .data = *from_hex("001122aabbcc")},
        false,                                               // odd_y_parity
        18,                                                  // r
        36,                                                  // s
        0x007fb8417eb9ad4d958b050fc3720d5b46a2c053_address,  // from
    };
    nlohmann::json j1 = txn1;
    CHECK(j1 == R"({
        "nonce":"0x0",
        "chainId":"0x1",
        "yParity":"0x0",
        "gas":"0x0",
        "to":"0x0715a7794a1dc8e42615f059dd6e406a6594651a",
        "from":"0x007fb8417eb9ad4d958b050fc3720d5b46a2c053",
        "type":"0x1",
        "value":"0x0",
        "input":"0x001122aabbcc",
        "hash":"0xe976a1c7600ed37c7aeea9b34de01b2424a68a4c9dfb0a0315a3db3cd9975512",
        "accessList":[],
        "r":"0x12",
        "s":"0x24",
        "v":"0x0"
    })"_json);

    std::vector<silkworm::AccessListEntry> access_list{
        {0xde0b295669a9fd93d5f28d9ec85e40f4cb697bae_address,
         {
             0x0000000000000000000000000000000000000000000000000000000000000003_bytes32,
             0x0000000000000000000000000000000000000000000000000000000000000007_bytes32,
         }},
        {0xbb9bc244d798123fde783fcc1c72d3bb8c189413_address, {}},
    };

    silkworm::rpc::Transaction txn2{
        {
            {.type = TransactionType::kAccessList,
             .chain_id = 1,
             .nonce = 0,
             .max_priority_fee_per_gas = 20000000000,
             .max_fee_per_gas = 30000000000,
             .gas_limit = 0,
             .to = 0x0715a7794a1dc8e42615f059dd6e406a6594651a_address,
             .value = 0,
             .data = *from_hex("001122aabbcc"),
             .access_list = access_list},
            false,                                               // odd_y_parity
            18,                                                  // r
            36,                                                  // s
            0x007fb8417eb9ad4d958b050fc3720d5b46a2c053_address,  // from
        },
        0x374f3a049e006f36f6cf91b02a3b0ee16c858af2f75858733eb0e927b5b7126c_bytes32,
        123123,
        intx::uint256{12},
        3};
    nlohmann::json j2 = txn2;
    CHECK(j2 == R"({
        "nonce":"0x0",
        "gasPrice":"0x4a817c80c",
        "chainId":"0x1",
        "yParity":"0x0",
        "gas":"0x0",
        "to":"0x0715a7794a1dc8e42615f059dd6e406a6594651a",
        "from":"0x007fb8417eb9ad4d958b050fc3720d5b46a2c053",
        "type":"0x1",
        "value":"0x0",
        "input":"0x001122aabbcc",
        "hash":"0xae1aea7493cc9a029710b601f62538993ebc6281ac63a241b83a218bd060b291",
        "r":"0x12",
        "s":"0x24",
        "v":"0x0",
        "blockHash":"0x374f3a049e006f36f6cf91b02a3b0ee16c858af2f75858733eb0e927b5b7126c",
        "blockNumber":"0x1e0f3",
        "transactionIndex":"0x3",
        "accessList":[
            {
                "address":"0xde0b295669a9fd93d5f28d9ec85e40f4cb697bae",
                "storageKeys":[
                    "0x0000000000000000000000000000000000000000000000000000000000000003",
                    "0x0000000000000000000000000000000000000000000000000000000000000007"
                ]
            },
            {
                "address":"0xbb9bc244d798123fde783fcc1c72d3bb8c189413",
                "storageKeys":[]
            }
        ]
    })"_json);
}

TEST_CASE("serialize EIP-1559 transaction (type=2)", "[silkrpc][to_json]") {
    silkworm::Transaction txn1{
        {.type = TransactionType::kDynamicFee,
         .chain_id = 1,
         .nonce = 0,
         .max_priority_fee_per_gas = 50'000 * kGiga,
         .max_fee_per_gas = 50'000 * kGiga,
         .gas_limit = 21'000,
         .to = 0x5df9b87991262f6ba471f09758cde1c0fc1de734_address,
         .value = 31337,
         .data = *from_hex("001122aabbcc")},
        true,                                                                                                    // odd_y_parity
        intx::from_string<intx::uint256>("0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0"),  // r
        intx::from_string<intx::uint256>("0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a"),  // s
        0x007fb8417eb9ad4d958b050fc3720d5b46a2c053_address,                                                      // from
    };
    nlohmann::json j1 = txn1;
    CHECK(j1 == R"({
        "nonce":"0x0",
        "chainId":"0x1",
        "yParity":"0x1",
        "gas":"0x5208",
        "to":"0x5df9b87991262f6ba471f09758cde1c0fc1de734",
        "from":"0x007fb8417eb9ad4d958b050fc3720d5b46a2c053",
        "type":"0x2",
        "value":"0x7a69",
        "input":"0x001122aabbcc",
        "hash":"0x64ab530a48c64d248b85dd6952539cae03cad7a001ed32ba5d358aca20eef0a8",
        "accessList":[],
        "r":"0x88ff6cf0fefd94db46111149ae4bfc179e9b94721fffd821d38d16464b3f71d0",
        "s":"0x45e0aff800961cfce805daef7016b9b675c137a6a41a548f7b60a3484c06a33a",
        "v":"0x1",
        "maxPriorityFeePerGas":"0x2d79883d2000",
        "maxFeePerGas":"0x2d79883d2000"
    })"_json);
}

}  // namespace silkworm::rpc
