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

#include "validator.hpp"

#include <absl/strings/match.h>
#include <catch2/catch_test_macros.hpp>

#include <silkworm/db/test_util/test_database_context.hpp>
#include <silkworm/rpc/test_util/api_test_database.hpp>

namespace silkworm::rpc::json_rpc {

//! Ensure JSON RPC spec has been loaded before creating JsonRpcValidator instance
static JsonRpcValidator create_validator_for_test() {
    JsonRpcValidator::load_specification();
    return {};
}

TEST_CASE("JsonRpcValidator loads spec in constructor", "[rpc][http][json_rpc_validator]") {
    REQUIRE_NOTHROW(JsonRpcValidator::load_specification());
    CHECK(JsonRpcValidator::openrpc_version() == "1.2.4");
}

TEST_CASE("JsonRpcValidator validates request fields", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator detects missing request field", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
        {"id", 1},
    };
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Request not valid, required fields: jsonrpc,id,method,params");

    request = {
        {"jsonrpc", "2.0"},
        {"params", {"0x0", true}},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Request not valid, required fields: jsonrpc,id,method,params");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Request not valid, required fields: jsonrpc,id,method,params");
}

TEST_CASE("JsonRpcValidator validates invalid request fields", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", 2},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid field: jsonrpc");

    request = {
        {"jsonrpc", "2.0"},
        {"method", 1},
        {"params", {"0x0", true}},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid field: method");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", "params"},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid field: params");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
        {"id", "1"},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid field: id");
}

TEST_CASE("JsonRpcValidator accepts missing params field", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "debug_getBadBlocks"},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator rejects missing params field if required", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockReceipts"},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Missing required parameter: Block");
}

TEST_CASE("JsonRpcValidator detects unknown fields", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"unknown", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0x0", true}},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid field: unknown");
}

TEST_CASE("JsonRpcValidator accepts missing optional parameter", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    const auto request = R"({
        "jsonrpc":"2.0",
        "id":1,
        "method":"eth_call",
        "params":[
            {
                "from":"0xEF04bc7821433f080461BBAE815182E3d7bBb61A",
                "to":"0x4debB0dF4da8D1f51EF67B727c3F1c0eCC7ed009",
                "gas":"0x5208",
                "gasPrice":"0x2CB417800",
                "value": "0x229322"
            }
        ]
    })"_json;

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator validates string parameter", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({"0xaa0", "latest"})},
        {"id", 1},
    };
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string pattern: 0xaa0 in spec: Address");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({"0xga00000000000000000000000000000000000000", "latest"})},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string pattern: 0xga00000000000000000000000000000000000000 in spec: Address");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({"1xaa00000000000000000000000000000000000000", "latest"})},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string pattern: 1xaa00000000000000000000000000000000000000 in spec: Address");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({"aa00000000000000000000000000000000000000", "latest"})},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string pattern: aa00000000000000000000000000000000000000 in spec: Address");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({"account", "latest"})},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string pattern: account in spec: Address");

    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", nlohmann::json::array({123, "latest"})},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
    CHECK(result.error() == "Invalid string: 123 in spec: Address");
}

TEST_CASE("JsonRpcValidator validates optional parameter if provided", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "not-valid-param"}},
        {"id", 1},
    };
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(!result);
}

TEST_CASE("JsonRpcValidator validates enum", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "earliest"}},
        {"id", 1},
    };
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "latest"}},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(result);

    CHECK(result);
    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "other"}},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
}

TEST_CASE("JsonRpcValidator validates hash", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"}},
        {"id", 1},
    };
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
    request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBalance"},
        {"params", {"0xaa00000000000000000000000000000000000000", "0x06734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a44979"}},
        {"id", 1},
    };
    result = validator.validate(request);
    CHECK(!result);
}

TEST_CASE("JsonRpcValidator validates array", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "eth_getProof"},
        {"params", {"0xaa00000000000000000000000000000000000000", {"0x01", "0x02"}, "0x3"}}};
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);

    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "eth_getProof"},
        {"params", {"0xaa00000000000000000000000000000000000000", {"0x01", "invalid"}, "0x3"}}};
    result = validator.validate(request);
    CHECK(!result);
}

TEST_CASE("JsonRpcValidator validates object", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "0x1"},
                       {"terminalBlockHash", "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"},
                       {"terminalBlockNumber", "0x1"},
                   }}}};
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);

    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "0x1"},
                       {"terminalBlockNumber", "0x1"},
                   }}}};
    result = validator.validate(request);
    CHECK(!result);

    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "1x1"},
                       {"terminalBlockHash", "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"},
                       {"terminalBlockNumber", "0x1"},
                   }}}};
    result = validator.validate(request);
    CHECK(!result);

    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "0x1"},
                       {"terminalBlockHash", "1x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"},
                       {"terminalBlockNumber", "0x1"},
                   }}}};
    result = validator.validate(request);
    CHECK(!result);

    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "0x1"},
                       {"terminalBlockHash", "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"},
                       {"terminalBlockNumber", "1x1"},
                   }}}};
    result = validator.validate(request);
    CHECK(!result);
    request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "engine_exchangeTransitionConfigurationV1"},
        {"params", {{
                       {"terminalTotalDifficulty", "0x1"},
                       {"terminalBlockHash", "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"},
                       {"terminalBlockNumber", "0x1"},
                       {"extra", "extra"},
                   }}}};
    result = validator.validate(request);
    CHECK(!result);
}

TEST_CASE("JsonRpcValidator validates uppercase hex value", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0xF42405", true}},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator validates `data` field", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"method", "eth_getBlockByNumber"},
        {"params", {"0xF42405", true}},
        {"id", 1},
    };

    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator validates nested arrays", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    auto request1 = R"({
            "jsonrpc":"2.0",
            "method":"eth_getLogs",
            "params":[
                {
                    "fromBlock": "0x10B10B2",
                    "toBlock": "0x10B10B3",
                    "address": ["0x00000000219ab540356cbb839cbe05303d7705fa"],
                    "topics": null
                }
            ],
            "id":3
    })"_json;
    JsonRpcValidationResult result1 = validator.validate(request1);
    CHECK(result1);

    auto request2 = R"({
            "jsonrpc":"2.0",
            "method":"eth_getLogs",
            "params":[
                {
                    "fromBlock": "0x10B10B2",
                    "toBlock": "0x10B10B3",
                    "address": ["0x00000000219ab540356cbb839cbe05303d7705fa"],
                    "topics": "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794"
                }
            ],
            "id":3
    })"_json;
    JsonRpcValidationResult result2 = validator.validate(request2);
    CHECK(result2);

    auto request3 = R"({
            "jsonrpc":"2.0",
            "method":"eth_getLogs",
            "params":[
                {
                    "fromBlock": "0x10B10B2",
                    "toBlock": "0x10B10B3",
                    "address": ["0x00000000219ab540356cbb839cbe05303d7705fa"],
                    "topics": [
                        "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449794",
                        "0x76734e0205d8c4b711990ab957e86d3dc56d129600e60750552c95448a449795"
                        ]
                }
            ],
            "id":3
    })"_json;
    JsonRpcValidationResult result3 = validator.validate(request3);
    CHECK(result3);
}

TEST_CASE("JsonRpcValidator validates spec test request", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    const auto tests_dir = db::test_util::get_tests_dir();
    for (const auto& test_file : std::filesystem::recursive_directory_iterator(tests_dir)) {
        if (!test_file.is_directory() && test_file.path().extension() == ".io") {
            auto test_name = test_file.path().filename().string();
            auto group_name = test_file.path().parent_path().filename().string();
            SECTION("RPC IO test " + group_name + " | " + test_name) {  // NOLINT(*-inefficient-string-concatenation)
                std::ifstream test_stream(test_file.path());
                std::string request_line;
                if (std::getline(test_stream, request_line) && request_line.starts_with(">> ")) {
                    auto request = nlohmann::json::parse(request_line.substr(3));
                    const auto result = validator.validate(request);
                    if (!absl::StrContains(test_name, "invalid")) {
                        CHECK(result);
                    }
                }
            }
        }
    }
}

TEST_CASE("JsonRpcValidator engine_newPayloadV3: patch blobGasUsed regex", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    // blobGasUsed regex at commit 5849052: "^0x([1-9a-f]+[0-9a-f]{0,15})|0$" does not work for input "blobGasUsed":"0x0"
    // blobGasUsed regex patch: "^0x([1-9a-f]+[0-9a-f]*|0)$"
    auto request = R"({
        "jsonrpc":"2.0",
        "id":13,
        "method":"engine_newPayloadV3",
        "params":[
            {
                "parentHash":"0x448aadba99a68c0c761f85d786d9aa1972af991b1cd542421c7a4f4a62987cc4",
                "feeRecipient":"0x0000006916a87b82333f4245046623b23794c65c",
                "stateRoot":"0xd34d72e16fadc36e7d1173ee3d497f7a07e3d0d2c59298653d2c40d10810aff8",
                "receiptsRoot":"0x08794b3ff9f42df2954a0c4913b1370c2a9294f5803ce1e6ea7d92091f30762e",
                "logsBloom":"0x0ca18204900040848c30020aa0001c20a0201c88000480002a504c0a4028c040040000088220a041c8012a4041840082808008c212a4a22005021001403c081804b080080c0d10084180402e0a8000008e022c8541844005442c000084200820c824040a46404210d2204a050402092440ca04c0c509d5a081009314841894940a40000c12018160a40001114cc0018000120921010400041a01109004d81800220920220a00c6061100834420301101624200200008082608c84c020800402941807003000000040083018d0802400061000008414003010001994040a4e22c20104084006200083000002020804000100810008c806250840a12120160c224",
                "prevRandao":"0x354265f4b5cbfe6d192e9ae4eed264b2dd5a7379f0a03d07ac3e01c1adfe164a",
                "blockNumber":"0x532187",
                "gasLimit":"0x1c9c380",
                "gasUsed":"0x6853ff",
                "timestamp":"0x65ec2540",
                "extraData":"0xd883010d0d846765746888676f312e32322e30856c696e7578",
                "baseFeePerGas":"0x6dd380a",
                "blobGasUsed":"0x0",
                "excessBlobGas":"0x4cc0000",
                "blockHash":"0x1e11ea7472b7d08d393bb5e301f15cd0bce394dd48956e4f629a194edfeab68b",
                "transactions":["0xf88f828317850ba43b7400832dc6c0944873528341d33ec918c7465f244491acb75bc95f80a4a0712d6800000000000000000000000000000000000000000000021e19e0c9bab24000008401546d71a08fcaf736322dfc21a5b7f130f6322498f4d3edefe5736ed9ec2e4fa62416cfeba0277ce00b907f9a51e7244d7057cf87b82c51d663bea88f081247f3e89c811b8d","0x02f8d883aa36a78301796885037e11d6008503c04cb416830493e09419fc4f304c1198c8ae1c23630a1611b27883693a80b864127e94515b9f10064bf8d747203a7b8070ff7d8268fc52d28fa25520f3b52902b1c9122a0000000000000000000000002c718a8053b649b29e23e833e18ba4412952c14bfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe70c080a07680861821d92cac56fe18783f78165f8cbf0be4f59f8f19370de7cd247a3a31a07db28cc5c9ae0873c8751edbc4ce79ff80aa9bfe8545ed94708aa06cbeb352ab","0x02f87a83aa36a783b38e928501dcd650008502a18851008255f094b9c8323843cec57ffd37ddf0105e028113c2b43d8806f05b59d3b2000080c080a0712d3d1d37881fba58cd7f08df9ff50507b26d7849c6c90fea6a72675d728b94a0060d9d9274e30dabf507854bfaa19b15405889c4859af757a23e6f0754440391","0x02f87a83aa36a783b38e938501dcd650008502a18851008255f094965ee94053e61d842106d8da5cb9c7eddefeadb38806f05b59d3b2000080c001a0a1925c0cf61486d900e01a7052616edccdcb5d057c289b6b405491cdef82ff51a049a1684742574bdfa59237579cb8bc85eb40709f14300c6b4697524b730a8c26","0x02f8b483aa36a78085013023a14485013023a14482d5089408394e7e653472694ecd4527656b2881e5701a1480b84440c10f19000000000000000000000000c799fe168e3f2253b1b2fc18fad38b5f8951a7af00000000000000000000000000000000000000000000000000000000000f4240c080a0673fdcc2f19c59293d45fd7120b008064f70573904848285c25fb64cc6e8b85ea02c4ccd33efeba5daee60a65ecbe2b2fe5ae7a35db63286fe9d42d021e4f313a5","0xf905af8085010a2626b083064755947a4ee6f9f0ab037fe771fc36d39c1e19bcc0fdb580b905442cffd02ea9660a6855e021cf4588c9c14180165077e80d2f3954532c643043bace534446b18270f38c40bbc2151a7cd7f3e64ce0bfa53ad392959c11a19a356965e05bae43d250cb0e326462a312d5d5539b2c817ccd521b39584e08520c3d2a709ee61c13897d4ac56f4b065b591f607caa3e146f2fca4fe5814ca0dd634c0ea53a171d905c4dd3f6874d88d4f6b444f9bdd0637b57efc2ec6d1a0be06def9e621784b6c5e01b451030430c21dc0aad9e7aefbe58a067d4fc87477c25eb70060c9d2e185117fb4fefb349d846eb9d416846d7725656e9e6d3c4c4d44a6dce200e174c7553f12436e41c0eef48f2dd40954d0fa7f6a302d4a2eb5c7858604fc4d517fa08174386e69569ec7aefbaeaef72301e8da5ab2d5ded043c951fb009f3fc7c6a277016e92de2ce5df5a8f45e27710270250b9ef0cd6c9bef354bd077a11af9c5dfa35115e48f885e004843ef5e5a938b05f5cd4480bcb3ad8970cade56279ea0468536ffc831b28f6aaef8d86b8204afdfd2811c8bfdc81ada405f61395be3a361a58aec64d206183cf926f03c27d5217572bf1f0562363891aa4877078867179e1961cdc85bed154b619ea983f96fd27ec4bd344bbeac04e347a10e4f1d2dee0b5c67add7c6caf302256adedf7ab114da0acfe870d449a3a489f781d659e8beccda7bce9f4e8618b6bd2f4132ce798cdc7a60e7e1460a7299e3c6342a579626d20ab52f279459cb7788f5aef6d2457ee4175e525aeb488dcc0345029c7b54f16829221f43f7a2eee4b69b6d7d152f0ba5ec50e75f0cb1bc9c38e5f18aa065650b5a2dce0a8a7f68bb74560f8f71837c2c2ebbcbf7fffb42ae1896f13f7c7479a09f054ebab2ee57f0c205353c5f4aba3e442ac4d4c09119801587928b85471765c65e9645644786b620e2dd2ad648ddfcbf4a7e5b1a3a4ecfe7f64667a3f0b7e2f4418588ed35a2458cffeb39b93d26f18d2ab13bdce6aee58e7b99359ec2dfd95a9c16dc00d6ef18b7933a6f8dc65ccb55667138776f7dea101070dc8796e3774df84f40ae0c8229d0d6069e5c8f39a7c299677a09d367fc7b05e3bc380ee652cdc72595f74c7b1043d0e1ffbab734648c838dfb0527d971b602bc216c9619ef0abf5ac974a1ed57f4050aa510dd9c74f508277b39d7973bb2dfccc5eeb0618db8cd74046ff337f0a7bf2c8e03e10f642c1886798d71806ab1e888d9e5ee87d0838c5655cb21c6cb83313b5a631175dff4963772cce9108188b34ac87c81c41e662ee4dd2dd7b2bc707961b1e646c4047669dcb6584f0d8d770daf5d7e7deb2e388ab20e2573d171a88108e79d820e98f26c0b84aa8b2f4aa4968dbb818ea32293237c50ba75ee485f4c22adf2f741400bdf8d6a9cc7df7ecae576221665d7358448818bb4ae4562849e949e17ac16e0be16688e156b5cf15e098c627c0056a900000000000000000000000000000000000000000000000000000000000b22a94f381e25913932c9e9345b1fda2c4c205b0b756fe043339ee1c1112f990a506149d2e86f16921f14cb603b6e08088010b4bd71e860d3f63ca479561914236f9d00000000000000000000000000000000000000000000000000000000000000000000000000000000000000003f4b6664338f23d2397c953f2ab4ce8031663f800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000cbbae7738710471b3c2acfd69d13621a449b30c200000000000000000000000000000000000000000000000002c4f3c3e8242680000000000000000000000000000000000000000000000000000000000000052000000000000000000000000000000000000000000000000000000000000000008401546d72a052a1fb4625957991f47c738aebb7686b31be96d5de7c280f6eb4e08b5badf267a06e30b89f5f594acb75cbb4eadc183fd4a78a8fc9a1776f10e990fec1bc9a6118","0xf901d58085010a2626b08309eb109419c7680f666e51a6086c270e2aa2a517ae585d05860d0c55ce6bc2b901642e325e040000000000000000000000000000000000000000000000000000000000000002000000000000000000000000b6f4a8dccac0beab1062212f4665879d9937c83c000000000000000000000000000000000000000000000000058d15e176280000000000000000000000000000aee05bf707311799e707b7ff67df6386d7311800000000000000000000000000aee05bf707311799e707b7ff67df6386d731180000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1bfe2cad7ef4e6a70d595a8c8d26e5b1dd1fc77f95481d29932f3fe5eb06158e2b74d9bb185b5a551c093e7831d262d3d0ec7c6f462744c96ecb3379fc53f33de6000000000000000000000000000000000000000000000000000000000000008401546d71a091e2ff4ccd0418066c1815be976ca57c01882f8a12057edf549f8c75138fd0a5a07712e8dab636b146bdb3381a839c2e62b97d9a026d9596687f800abbaf55a9f7","0xf874830c35b8850108eb78588301482094914e58348ec3bc0f6b9658a2c43b258323ec8eb68806f05b59d3b20000808401546d72a095731ec36b6f1ce0bb043c627f16db8c873d38da67aabf7133ea5cc679fa1de5a03b985c9348a17ed4ce9b07dbc0a0c6ebf210a1743dfb918256e2ecc70ac57254","0xf874830bb8d1850108eb7858830148209420275c43e677586f48ef37158dda42f80d45cd9d8806f05b59d3b20000808401546d71a0726172ce6de61122bc338269d832427b8527bd244377f88cbd199ef9a21b9289a0566dc985a32a2d001071f07cf9b626751c4817c2d72a1a44de0bd1eaffb4d454","0xf874830af78c850108eb78588301482094a55b483606c5ec9d92e25e6b139935329ec2d28a8806f05b59d3b20000808401546d71a02e878d2572aa378c7f24acd39b23cff51608db8b5bd8acb666431b1a83242e59a0768083ae736ff5243b832ada957494cd43f86d7edc2c4901c85f80b2cb1a1ab9","0xf874830ade7e850108eb78588301482094ed5612624e2fd5f5954137b41e8f7a3f5378cbd78806f05b59d3b20000808401546d72a02020638007001378729eeacaf525d665d7f8b0b66ee47b00100a65c632607156a0110cb949fe9ebe13a6cd0ed00a6f69caf8e737a8bdc72e8e885620d4006f0560","0xf874830cd996850108e6d90c83014820949aed55be7ac0d3727e4dcc1073746a74598bae428806f05b59d3b20000808401546d71a0da95b9bf7fc24e4e562b27223183bde5e99cebab518b69001f80b3bc78cac54fa05edce4c2defa58703dcb368792aeeebde328a949028e01ee85e41c74a4c335a4","0x02f8b283aa36a72784f50a144384f50a144382b63e9401fa8deeddea8e4e465f158d93e162438d61c9eb80b844095ea7b3000000000000000000000000321ce961084fcf3a56de4be2f2006707a0421aa4000000000000000000000000000000000000000000084595161401484a000000c001a04d9829627dda62ad7561b50fe2eabe3be64016f0c1c48eac6554589f1332f670a065db2043c2cb2052e0e1a3de190510cb5ef8c92014cc437f4d62649bce48bb2a","0x02f905b783aa36a78311cbe384b2d05e0084c110e76a83041ebb94bb5ed4265de177516699f59bddab841ddde97e5e80b905440a321cff0000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000002c751904db0719b9d1b4947952711a2bc901cfa0cc199e16e5eda419e963723ca750180000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751a09167ce4f7827d9e3c9825beef93121ed72130a23646f3facee1b5ee5f5d19310000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751b4cc9abfe559ccab33710c14d7633469fefb4f05c483af6e1fd7035f8c09e2a350000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751c2442cf83f2631faf24d147d000d356929537f4a72abece35e1cc5b89dcaad3f00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751d15cc2178993c7a3fcc1b95b3923f8e54f76720f44396691949d8762e4935b2ed0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751e86b7423a174fb76f2c95c0b12b907124b37a86ef69fd4aeef0fcf8d67a5c8e890000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c751fad4d19f0234c5e0ec9f03379fde620916768601bb4bf838a60a5f924c2594e0b0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c75201b4bb2191691b5c0e5db54165741b71dda83e7b2ff9bc656c060f7d9310e748f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c7521942477a8b505df86695acbefd377b8973cda132339b7f9b9eb1161de45a9834f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002c75225d1c1cdca8c4feba22e7e9e0a0c9b670e79746c121f2f6ce8454638a12e6689c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c001a047d3de245128c7648be55a47ce1ad9eb8e05dee7eff13c567a0618482a20df07a073346378bcbd6c2cb87aa01adddc606b1ef0a91b7b60b0f570a66ae816c91774","0xf904108229948484736c86830327169433cafa2e5d5167806b236f5be9734ddce8706bfc80b903a42b0006fa000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000072dd00000000000000000000000000000000000000000000000000000000000072dec69080ecfe089df86a093d59903f41924e0995cbf084044031c698ea57270b42235b3a48ca14d2f68266f341124221593009649c5e7115e80243a83d5738b3ad20227cbcef731b6cbdc0edd5850c63dc7fbc27fb58d12cd4d08298799cf66a0512c230867d3375a1f4669e7267dad2c31ebcddbaccea6abd67798ceae35ae7611c665b6069339e6812d015e239594aa71c4e217288e374448c358f6459e057c91ad2ef514570b5dea21508e214430daadabdd23433820000fe98b1c6fa81d5c512b86fbf87bd7102775f8ef1da7e8014dc7aab225503237c7927c032e589e9a01a0eab9fda82ffe834c2a4977f36cc9bcb1f2327bdac5fb48ffbeb9656efcdf70d2656c328903e9fb96e4e3f470c447b3053cc68d68cf0ad317fe10aa7f254222e47ea07f3c1c3aacb74e5926a67262f261c1ed3120576ab877b49a81fb8aac51431858662af6b1a8138a44e9d0812d032340369459ccc98b109347cc874c7202dceecc3dbb09d7f9e5658f1ca3a92d22be1fa28f9945205d853e2c866d9b649301ac9857b07b92e4865283d3d5e2b711ea5f85cb2da71965382ece050508d3d008bbe4df5458f70bd3e1bfcc50b34222b43cd28cbe39a3bab6e464664a742161df99c607638e415ced49d0cd719518539ed5f561f81d07fe40d3ce85508e0332465313e60ad9ae271d580022ffca4fbe4d72d38d18e7a6e20d020a1d1e5a8f411291ab95521386fa538ddfe6a391d4a3669cc64c40f07895f031550b32f7d73205a69c214a8ef3cdf996c495e3fd24c00873f30ea6b2bfabfd38de1c3da357d1fefe203573fdad22f675cb5cfabbec0a041b1b31274f70193da8e90cfc4d6dc054c7cd26d09c1dadd064ec52b6ddcfa0cb144d65d9e131c0c88f8004f90d363034d839aa7760167b5302c36d2c2f6714b41782070b10c51c178bd923182d28502f36e19b079b190008c46d19c399331fd60b6b6bde898bd1dd0a71ee7ec7ff7124cc3d374846614389e7b5975b77c4059bc42b810673dbb6f8b951e5b636bdf24afd2a3cbe96ce8600e8a79731b4a56c697596e0bff7b73f413bdbc75069b002b00d713fae8d6450428246f1b794d56717050fdb77bbe094ac2ee6af54a153e2fb8ce1d31a86c4fdd523783b910bedf7db58a46ba6ce48ac3ca194f3cf2275e8401546d72a0c28451ec7799c6a1692b8b5fbdc3982a7af3a4ffea90db1a67ac6e12f4610966a05d5a773b7ad73c5fbc4e57e715ea902cc8028a4d926b2166c1450e4c9416a674","0x02f87883aa36a7830c1796847735940085746a5288008252089423c5f78b35b945163b7c4834cacf3cb03d36c2b587b25953d844290080c001a0b743453784794ece06aab5674a7e2a00298f8c7bab4ff9b4234f0f946d2e1f0fa03cba8b238e4b3c67c3513cb98e281c42f307d9d16d6604e2e61a3a09035b968b","0x02f87883aa36a7830c1797847735940085746a52880082520894bfcdba8331d02cf265448bbfdd338ddd6cca263087ba44cf3a95d90080c001a0905c6f775e0bc7150f83373ea680743e064994266ab4853a1766e2730a15d8f3a00d96c43cc42ef11f9fcd778b882a4fb18bd704655a29b1a9f849e609f4214dd1","0x02f87983aa36a7830c1798847735940085746a52880082520894e7af3966a2303cbaab0d824b7acbe6518ebfba1a881d5d0f774ceb120080c001a056c850281ab9e3dfd788e64af2cd1f09ff259b2f06bb12969f3acd45a626b3d5a054582a8567c3800bf2a49b53aaa215dbd19ee2e038af34fb4fe975706176d754","0xf86f048460d26080827b0c941ed29c10e661e5721dfe162845f72548f090d8e78803d0ff0b013ba544808401546d71a0280f65b44c3a91a61d0a193ab6c6e80a7c5f071498386a4403626ad17a7f819da0274cb676983621230c472ef749061039ae88e3d33f5f08cd219ff5984051623b","0xf86f018460d26080827b0c941ed29c10e661e5721dfe162845f72548f090d8e78803d0ff0b013ba544808401546d71a0d37e39fc4c4dd84af425a5123d48f4c6369c5bf388f4e9984683c01abfba8e90a017cf0dff5dc0c280d4b562d742950774fcc6144648fb4c85906ca3c0e7385123","0x02f905f583aa36a70c8459682f008461d00b6f8401b2e02094557c5ce22f877d975c2cb13d0a961a182d740fd580b90584cd88ec5300000000000000000000000000000000000000000000000000000000000001e0492d0141712e199130278448ec1298e16b90384d089e794a00c8bc91258405f6000000000000000000000000000000000000000000000000000000000063b3700000000000000000000000000000000000000000000000004000000065eaf7bfdfd244b5be34274673663e674f44914fa23660647fc758955b7a0881a1bc7d331ce5e76407d7232ac2c6a888fb043f5d73fd220cb78b981629fb735b348a960d000000000000000000000000000000000000000000000000000000000063b1b20000000000000000000000000000000000000000000000004000000065eaf5fe000000000000000000000000000000000000000000000000000000000053086e000000000000000000000000000000000000000000000000000000000000000900000000000000000000000000000000000000000000000000000000000000011f3555d41f43859ff7fbd1c0b283f532d8b4b7161e79d621390a65a1ad855704cbaa45f6b7e3e50eeaa3a3e134eadb6a822440eebb25ea92b0709f9c74266b7000000000000000000000000000000000000000000000000000000000000002a0000000000000000000000000000000000000000000000000000000000000052013a13f379a0ef7aad2be353f07a4a575caca320acef9479f7b0843785d9379c2000000000000000000000000bcd3b2e47e5c2f8528a17a6e047745a4c50c017e7e5b7621bc6c5be7b18b6e3e1c8b0f7c8df53a33c4640eb26318b5caad85ea7000000000000000000000000000000000000000000000000000000000000f424000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000063b1b200000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000011d19623a39c967a474746d3ffa6fdfee9cc5b6bde3059b548b04f48bec7c94ec73b2c3848c633015ca66f3910855f9deceeec865a7602e0c17125acb113aaebcd63d4ab8a6995ad4bb40998cb3917d18c38e71a07783e1b6c1b30fb50602661b436480d0bb220364bfccac2478a0e8b1556830e78bda0b3eab61e13d8a1a519105a9813dd3bdc06caafe8e2dea9188e30ce34ae6e1f11f3dde4e70acc6ba8d560d77bbc3b6fcecd4de639dfa6f799504fdbbc7981fb379a45b2efd894e95fcdcecad44b7b7d2870edaab79c90c6e56e9f2e58190b65282c7e5aa233962dc2b19706643a0578c1209a952208f13294db5eb047ef8fa10d24ab4eb91f1f38df6eb74562123e9158f706a2d8d1250e28085dffa4247dd08b90b3ba4c5d0dc708e88e60e043f4262682993b65adf322dff9bb79922974eeb5ea4752dc56391064737a9a7d57e1afdc2630fc378b7ff791e4285f63c45381e021aad42925721a5b203b3a8b84fe2577dbabb90bb1ca1af65f3de40e385ec5aa72120fbdc8f0e76b1c1b57d3cdbe3e979959c837d0f85d983c4b20ae560d7bb3b3200af8720b2f1a1ae987d67093ac1c77c24bf1d92d06cbdabccfb685b62570dacdc5af44c31471a026801703661388c4cb7df3b50de1bd9c8136b6ca52ef90f6e37a9afab26922697192ed73610803acd5c8e8efb75a46ca5f5d2f0676fd38410ff194c53aad1a98f79f2e4570a38828ee8a033033675549eee462dbc76a7da5d7332b444fd7a27c25000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000c001a0aef0bda18a81d64adab460039e018f63a0ba324267bf4b79be83b90573ce7e26a051facb64ba9f6c5f855563ffde1061de4ff9b0d4b9d92fb1b11e23eae2fc5601","0x02f9017683aa36a781b88459682f008461d00b6f8401b2e0209419c7680f666e51a6086c270e2aa2a517ae585d0580b90104faa9bce90000000000000000000000000000000000000000000000000de0b6b3a7640000000000000000000000000000b31c077c99e0d61305fcd873a39f974d13bc773c00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1b45188ccb74751133eb81ec5f059fd3101ee73ebafee8ea007407f5d0f30c50ea5240cd8f54223e2bf9d45da6c98feb9d40fd6bb18b67eb0ac2ea23e587874cbd00000000000000000000000000000000000000000000000000000000000000c001a0ea6c2fd48a0d181544124b58e8d0ebab38b407dd5c6519e61c5f074311f8f34ba03b731a85472942ea0267af3b71cc58f7c5fb2931099f2526f3a3105c939e2493","0x02f902fc83aa36a7068459682f008462d9f23f8302f63b943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad8809b6e64a8ec60000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec277400000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000009b6e64a8ec600000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000009b6e64a8ec60000000000000000000000000000000000000000000000000000018e06363c66e3eb00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c001a04ab810c66296497f7d275c8cbdf7ebc5a91ea14f9341316e31ec7949094d4e20a009f15f6e9a4dd182d81ae5bd08340ebcdcd7492e267070bf35b30eb44f39a418","0x02f9043d83aa36a781848459682f008462d9f23f830426a0943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad88016345785d8a0000b903c43593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec278000000000000000000000000000000000000000000000000000000000000000030b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000001e000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000016345785d8a000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000010a741a462780000000000000000000000000000000000000000000000000000014ac026617ccfe00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b14002710b31c077c99e0d61305fcd873a39f974d13bc773c000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000058d15e176280000000000000000000000000000000000000000000000000000006d4816e85fd5900000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140001f4b31c077c99e0d61305fcd873a39f974d13bc773c000000000000000000000000000000000000000000c001a05c299945a3a544d22733ff66bbec3030092c91911dd3b5f4ce68c18dc47a6478a07a7114ebcd795a537b296b4d20fe1c77f4afe63a41f889b290f277ef86c9bda9","0x02f902fc83aa36a73f8459682f008462d9f23f8302e8fc943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad88077e772392b60000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec277400000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000077e772392b6000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000077e772392b6000000000000000000000000000000000000000000000000000001330cd97b0d4ef900000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c080a0a8fe19cc48a5c5b041d6fb96c4283a6f38bd7475d4ca1f37c77409c26ce68a40a04aa9db8ba0200dd02fb5cbf926a5619f6a34f7b2ca35d122256157da0a41b21f","0x02f8b583aa36a78201058459682f0084630562cb8302d7f694b31c077c99e0d61305fcd873a39f974d13bc773c80b8447bde82f200000000000000000000000000000000000000000000000002c68af0bb1400000000000000000000000000008334b2380a85d14f4366bfd52634a083382639adc080a0a0a2f59bad445d33b33a0179abb498751e5cea7799d8de66806ad79991146e4fa03b2917c3ac96b335b87959c8888caa236904a28b1e2c2f076910733874ae75ab","0x02f902fc83aa36a7028459682f0084630562cb8302f63b943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad880de0b6b3a7640000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec277400000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000de0b6b3a7640000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000de0b6b3a7640000000000000000000000000000000000000000000000000000023898944434e94e00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c001a063d5d7fad1c62eb2dd1e01dbdaae93cf845a851fece39c6bd74f9cd155db0990a0667f8246c6a198632d32f4495aa8ef89ce0137c0375a38ec88d3c3ceb0468de3","0x02f902fe83aa36a78201388459682f0084630562cb830292b7943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad88016345785d8a0000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec278000000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000016345785d8a000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000016345785d8a00000000000000000000000000000000000000000000000000000038dac33485e30c00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c080a01d35d727bc6611daca011098e403081c611b94cb58a644ac1617005c95541083a037c296c97d1617b044c91000caaed68fa7f81dcdb96e8d09d15f9b408f36e22e","0x02f87483aa36a7018459682f008462d9f23f825208945e809a85aa182a9921edd10a4163745bb3e3628487160ff64c0e33a980c080a0179beb93e9b7f9200b08dd7e8473b5aa5d6e2757b389e771f68ac6e0b3e85c57a0131cc6d17980908975aa53305ab35391d7c8a40755fc74131f5431bed77c0bb7","0x02f87483aa36a71f8459682f008462d9f23f8252089446e53434b77c7a301b54ff4f01b3c05f5688576687b1a2bc2ec5000080c001a08c29517e09da96947a545ee66cec1bb8f833ac711aa0460673798244212e2600a0723eb2fa32e194210220febd93652d66f04c59527405d563cb0883e6d289feeb","0x02f901da83aa36a70d8459682f008461d00b6f8309eb109419c7680f666e51a6086c270e2aa2a517ae585d05860c1d93b2e642b901642e325e040000000000000000000000000000000000000000000000000000000000000001000000000000000000000000b6f4a8dccac0beab1062212f4665879d9937c83c00000000000000000000000000000000000000000000000000071afd498d00000000000000000000000000002a7306d74ec5cc82ed3685d387fadcc5ed0c06550000000000000000000000002a7306d74ec5cc82ed3685d387fadcc5ed0c065500000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1c7ec5692ccfc700dc340b3902a8e4f8d1dab0c4fc9fa4541d2ca64d49d36b158d6af6e5e21548b6d0fac426bf034b88ce44d080f0be4d985b0e6aa0431b6da63c00000000000000000000000000000000000000000000000000000000000000c001a0890e8992dc344f0929e1c5af123aafd2320303a9b328d287a1fde98ecb2d6b2fa07ad9f35d65f83d79955e3ec449e1121707108d421854a4dcf60b0b9143a012dc","0x02f8db83aa36a7118459682f0084630562cb830e6f0d941b240fa1e884cb5e09a7a22edf2563f371c5f8828803782dace9d90000b864b1a1a8820000000000000000000000000000000000000000000000000000000000030d4000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000c080a0874bf3789870b7a8b18cd54bc76860f561ab829cce28ae8f0e84fe907c9ac0c5a00dbeee510cab64436466a4af0316aad0b92aea33ca430ea2dabc95d3c70579b2","0x02f87483aa36a7018459682f008462d9f23f825208945e809a85aa182a9921edd10a4163745bb3e3628487166ae95c8873a980c080a0b2afd01021652e58153b0c5fb5ed0eeb67e597cd89cd56be94dbd16d99620878a06e32970f4ba0f1789dc5275f6f4cf21c8e60aa9980fee60eb607cfd99635e80d","0x02f901da83aa36a73e8459682f0084630562cb8309eb109419c7680f666e51a6086c270e2aa2a517ae585d05860d0a3822d182b901642e325e040000000000000000000000000000000000000000000000000000000000000002000000000000000000000000b6f4a8dccac0beab1062212f4665879d9937c83c000000000000000000000000000000000000000000000000045cf1dfa23867bc000000000000000000000000d06c414d539fd06d3d1a13c2985a0b8c765e8363000000000000000000000000d06c414d539fd06d3d1a13c2985a0b8c765e836300000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1b42cea67aec5cae595656a61cbee1db9e1a3ec699cdd2188ad03e37ae1608c2315e5069a16331171fbb9a0425e4a8c5823d239fc14377e0da54f9bf041acdc99000000000000000000000000000000000000000000000000000000000000000c080a043eca376fdd4c0f062fd5068ebeec83c795f7c2d88b8d3d1a6126e5548876342a00774387c2fedb695b834b0169cd9b6ce19c032b9f645f528abf73d230e43f381","0x02f9019c83aa36a7428459682f008461d00b6f8301ad5394d13d092f9813f6afb9230cdb76f370ddccba20f08802c6e95663fe5f80b90124679b6ded000000000000000000000000c3aac133ba5895e0c930bad02023e342f3efc8fa00000000000000000000000000000000000000000000000002c68af0bb14000000000000000000000000000000000000000000000000000000005acba685af80000000000000000000000000c3aac133ba5895e0c930bad02023e342f3efc8fa000000000000000000000000c3aac133ba5895e0c930bad02023e342f3efc8fa00000000000000000000000000000000000000000000000000000000000493e00000000000000000000000000000000000000000000000000000000000c96a8000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000c001a075bdb8a54e1d1bca9b4307185caa37ba256c0ad73d4d1d8e6259313bdb0cb730a049c664610194ed1c1950f2a2163d606e036f4ec1d8d14ff53e797a0e52b834bf","0x02f87583aa36a7028459682f0084630562cb82520894f32d473da97b831c1025ba7eee80e57d4013c99b8803fb7c9980bf000080c001a009a5c41644a22facb3916251d3eeb22e85d2afa9ca341c46357ee5dfdfacd6bba00126b7ec1ec696d4dfa22c6caa99481dec90ad4b9200f9334a97a63eaccc5584","0x02f9017483aa36a7018459682f0084630562cb830755769419c7680f666e51a6086c270e2aa2a517ae585d0580b90104faa9bce9000000000000000000000000000000000000000000000000016345785d8a0000000000000000000000000000b31c077c99e0d61305fcd873a39f974d13bc773c00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1c443c831aff82b2dc0eaa537e42fe3087a084c35d39a123d8ff56886da7b0d8b2766fa00667ed35d48dce449a11b82b6c52c92ef6a2d0bffe014d2b37c651beee00000000000000000000000000000000000000000000000000000000000000c080a074b650718466a0a060b0bbd7da46a9393a6c14e4966f9c8a6f2155c12ce702f8a07dba87b7e4d1e64183ccf71f024c3ee4798f3a8b2488369463a38b073f29c541","0x02f902fc83aa36a7588459682f0084630562cb8302f5e1943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad881b0fcaab20030000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec277400000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000001b0fcaab20030000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000001b0fcaab200300000000000000000000000000000000000000000000000000000454b331f4c63a0600000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c001a0ea17b7721aa6bf3b022be99d0240892747417640e2b09aaa13d68d63a3811df5a06b1a92d6056b362cb6fb4c333e5599b654a8f9ce14ed2da76bcd52e85ff3a40f","0x02f87483aa36a7018459682f008462d9f23f825208945e809a85aa182a9921edd10a4163745bb3e362848715d04c26ebd3a980c001a0a4c2625ede867c6310896ccbe03fc157f143fb07ed038e65bd8c85de4f96055ba035363af57c6c158b1902d5a2608f8b51bf54583955db461cd842d899cd94671e","0x02f8dc83aa36a782048a8459682f008462d9f23f830248ba94b9a60416f2dea96c07a9ab53e35350888d2d67e08720a0125de6c000b864c25dc7bc0000000000000000000000000000000000000000000000000000000000000c2600000000000000000000000000000000000000000000000000038d7ea4c68000000000000000000000000000a19874046c0afdddf42757e094f45cbc0bfd649bc001a05fad3f331010a026faa7223c81431592107c21963f1d829c0fba5fae787045c9a023f9841ec4438c7129e631e1377c9af37e78660427e7fa91645a2ef4caa1c0a9","0x02f87483aa36a7018459682f008462d9f23f825208945e809a85aa182a9921edd10a4163745bb3e36284871619f76f25e3a980c080a0372888d87046421b7d60ac06b75d55d26e4679f7ca585106832035bbf6d7eb69a07934b938e5a1eb190f72c6917bb3ba0b2ba22833d2bc43ee98912a82993acf4b","0x02f8b283aa36a76e8459682f008462d9f23f82b652944b8eed87b61023f5beccebd2868c058fee6b7ac780b844095ea7b30000000000000000000000005699f1bc92b8e61aad266f658de9c968631105230000000000000000000000000000000000000000000000056bc75e2d63100000c080a0e3f8cf21c0c987e718839478bc4c255179e262f7a382d20a6da2e0804abe0214a062742564b43928aa00e7740109d0874670f6cb2165f8fdcdfecca2ed7e50fdaa","0x02f9017583aa36a781ae8459682f0084630562cb830689d19419c7680f666e51a6086c270e2aa2a517ae585d0580b90104faa9bce900000000000000000000000000000000000000000000000000b1a2bc2ec50000000000000000000000000000b31c077c99e0d61305fcd873a39f974d13bc773c00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1c2717b89df289ff56f2fa5d410cccf52904d7ec3b0f9e5b2c698e07be8486abb8524ff83ef5678d60846d0e4ccf7dd1bc84fb150e1d612ad492498820719f9b9c00000000000000000000000000000000000000000000000000000000000000c001a0cf19b29b5ade6cfd4d02efb07a19199c2130495e2a5394c84c6bb05b3f56bf89a037e8c423b623225a1cc3592aa42daef918e89ac6e5798c02ca22b64d189f04bf","0x02f902fc83aa36a7048459682f008462d9f23f8302e8ba943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad8806ccd46763f10000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec274400000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000006ccd46763f100000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000006ccd46763f100000000000000000000000000000000000000000000000000000114f435a31a2d0f00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c080a0877d6c5c52c51f85ec1b5fb474f4900ae77bc744c41819792cec739b5d15e82ca004a3ecfb91548c9b1f87cf1c9e119c82537970fb44aadd92cb12527cab011456","0x02f9017483aa36a7018459682f0084630562cb830689e39419c7680f666e51a6086c270e2aa2a517ae585d0580b90104faa9bce900000000000000000000000000000000000000000000000006f05b59d3b20000000000000000000000000000b31c077c99e0d61305fcd873a39f974d13bc773c00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1bba3de764afee699adb13ebc1be6c6823f67d1802230ad3fdbef6f01f7ad1738925f8fa08927ac8fbf2532dee91ed8ff7423614520db9f34c527d664026c1521500000000000000000000000000000000000000000000000000000000000000c080a02d0d667c36bbd9205df55e6ca26a4b98bd68936245931f1fdeceeb165a3e62c3a00b7b0193a5f22b85c428f50bef33a13316a2b947c60cfa3a0ee3127e607a1b01","0x02f902fe83aa36a78201388459682f0084630562cb8302a4e1943fc91a3afd70395cd496c647d5a6cc9d4b2b7fad880de0b6b3a7640000b902843593564c000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000065ec278000000000000000000000000000000000000000000000000000000000000000020b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000de0b6b3a7640000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000de0b6b3a7640000000000000000000000000000000000000000000000000000023883e27a56832d00000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bfff9976782d46cc05630d1f6ebab18b2324d6b140027100305ea0a4b43a12e3d130448e9b4711932231e83000000000000000000000000000000000000000000c001a07ee13472fef5d9c6be9054268b54d936189952058ab59ee2475c12d410ca5741a02cff07a888b29694e44f871eb92da5b1c521bbb8d321379da7238845922d9c40","0x02f9017483aa36a73e8459682f0084630562cb830689bf9419c7680f666e51a6086c270e2aa2a517ae585d0580b90104faa9bce900000000000000000000000000000000000000000000000000470de4df820000000000000000000000000000b31c077c99e0d61305fcd873a39f974d13bc773c00000000000000000000000000000000000000000000000000000000000000600000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1b88370c04e89a0cacdf03ae09222d305c59a42e2bfa9dfc603af6304a4fbf35b5401f0f55e94a6e10005d3a5eadb4b896a5c9ada5e9c692d1c16cfb1d131474ea00000000000000000000000000000000000000000000000000000000000000c080a0ae173cc9563fde7e7b296fd6454a7ffac1945b140884bd1bcc86c6407e2cb482a00985b112c17dd36aa3d0566afd4b1587bdc36e5bb9cf3eb7f3442e1acb641592","0x02f901da83aa36a7258459682f0084630562cb8309eb109419c7680f666e51a6086c270e2aa2a517ae585d05860d0a3822d182b901642e325e040000000000000000000000000000000000000000000000000000000000000001000000000000000000000000b6f4a8dccac0beab1062212f4665879d9937c83c00000000000000000000000000000000000000000000000001cdda4faccd00000000000000000000000000003d3d8cada4ae176801222fb6693520a5329fa9d10000000000000000000000003d3d8cada4ae176801222fb6693520a5329fa9d100000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000061ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff1c3edde24be770998e9575ae92af125aa62eca343f8bb772dfe85ac29ef2efdd25591bb1b845b80aaa55a1a14a5be2a7b45056d9c48b2827fc4ec67f2ced33991300000000000000000000000000000000000000000000000000000000000000c001a05970e773f8c58c74d514ccc8949bc628f5f2579b5ded158d0221c8eb7338a511a039f8087d69f5c0e1bfb5b38b221c6ddd17a698e192827c8d4819803f78c924f4","0x02f8bb83aa36a77f8459682f0084630562cb8302e34294ab260022803e6735a81256604d73115d663c6b82881e75a6c3083c7c00b844592de11d000000000000000000000000dfe738894d90efe91f796fd1fdfb6a486a90e2b80000000000000000000000000000000000000000000000000000000000000019c001a0586247735164f942bf0b5d4ce26078ac9856db2de25a31efab21095cd8a98ef0a0597610109db5296873fede2c95bdde8fc40b15ba3e07dedc1967d88d7d2bba94","0x02f9019d83aa36a781838459682f008462d9f23f8301ad4694d13d092f9813f6afb9230cdb76f370ddccba20f0880163a318baa32700b90124679b6ded00000000000000000000000093404bcb286f234286e1bb26bc79c298da89b63a000000000000000000000000000000000000000000000000016345785d8a000000000000000000000000000000000000000000000000000000005a065ab4770000000000000000000000000093404bcb286f234286e1bb26bc79c298da89b63a00000000000000000000000093404bcb286f234286e1bb26bc79c298da89b63a00000000000000000000000000000000000000000000000000000000000493e00000000000000000000000000000000000000000000000000000000000c96a8000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000c001a09952f5f1e24e916cc585bdd33a8f5182a99956648227d1b6524b700c1c2f093aa07a03cd1160caf36bbe7dd29386e4f5ebaab1a04fb7de3858c0b7347c6279ccef","0x02f8db83aa36a781908459682f008462d9f23f830248ba94b9a60416f2dea96c07a9ab53e35350888d2d67e08720a0125de6c000b864c25dc7bc0000000000000000000000000000000000000000000000000000000000000c2600000000000000000000000000000000000000000000000007a1fe16027700000000000000000000000000004c66a65fd9c7b0136fbf1ba4617521ba39379c43c080a08f4e752a49bcabd1197e21d8e74428ed8dd8478b64790248e4fb6b9e4460afbfa01559ac0d0980f5d0ed9bb779b7c2eaa277d3f997da3983011436f79ea052575d","0x02f9049783aa36a7830198cd8459682f008464d24f0e83a037a09486efbd0b6736bed994962f9797049422a3a8e8ad80b90424b1dc65a40001efcc06557badbcfc10367afacd814e090b9f7a3c8c3ccc3b96e0657b0ccd000000000000000000000000000000000000000000000000000000000c6f4500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000036000000000000000000000000000000000000000000000000000000000000003c0010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002600000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000007735940000000000000000000000000000000000000000000000000000120393b3cfc03000000000000000000000000000000000000000000000000000000000000000c00000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000014000000000000000000000000000000000000000000000000000000000000001e00000000000000000000000000000000000000000000000000000000000000001b4e304b0000000000000000000000000da6e31da65205210e5075a5076742056000000000000000000000000000000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000f42400000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000532186448aadba99a68c0c761f85d786d9aa1972af991b1cd542421c7a4f4a62987cc40000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002bcfda3bbd4dfd70cd70029be9736de5f301a4fe367f5bd4668d9b25a489abcb265a6bca73b2d91066debc1c583a9ffc4a4da23ab884d071f09aa47c60cb22bb700000000000000000000000000000000000000000000000000000000000000023498cff9e882a054d1661ce135ee2974c89925a57495940bbb5bbac8336a797072f98fff8644b98ce678fe0864d51864785d70e847d52cc44f7ccf6b5f82a6b1c080a0cc2b8e3b5e1aab744f5b55520fbc263d2de6653a00e03e201542f16c3f33ed79a0452e5759b15b6e28b2ac2b40b21834e39f58e01f1dccd4e2d8f5d0b70f840cc7","0x02f87783aa36a7358459682f0084630562cb83018f0994120de9fcc9b1cca7e071fbc9ee500a7b69980ec685018cf789008400000000c080a0b12bf5902aeb7b08323081ecd77a411eb88ff218c95c8da341b6fa89a8e1dd80a066eeb6edd6e17a2d2c66c25173c604a50cdc7f08d790499fc6748ed3573a0cb3","0x02f87483aa36a7108459682f0084630562cb82520894e927025831c569e48df971f6818b061910920a53872386f26fc1000080c001a0c0ae976389cedfe0f9420be80ca927ad1cbc537391c743690cce7affedfc3796a07624eb9c2307be1910cec58a036fb05f5a40c7ad4131764da97a83d0587515a5","0xf902708297c584561953e98313d3d494f68f872f0dde0ec1ba8c28eed9d0674760aa8eb180b90204e7d811960000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000018000000c2600aa36a700000000000000000000000000000000000000000008e3d8966c63d14939ec9ace2dc744f5ea970e1cc6f20f12afefdcdff58ed5d321637e0000000000000000000000008e7dffb6d9b89b3701dcf1ef49e30e90610c88cf00000000000000000000000000000000000000000000000000000000000000c0000000000000000000000000f2e50b1753cd1588e33f38bb78f1d609916d92bc00000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000d529ae9e86000000000000000000000000000030e3a4286e227968fe3c8957f61c568a55ef62480000000000000000000000000000000000000000000000000000000000000041ee2d46552384f2b47336fd4a3afd8084259259788b8ecbfff67d9c74b658245e5590cac521c6834af12e7dca863edae2465182dfbd678ae58690f57fd61de4301b000000000000000000000000000000000000000000000000000000000000008401546d72a028dddabef21aba0a51f6886777f973d2747f1e68c48775fba9eb8909af996be1a0532cc002d0b196f0225c7b9ffe525b236edc1e94bd9a337b6a28cfa43b992fe8","0x02f9025683aa36a782fb5d843e4b406a8447e75bab8302de9494cf8edb3333fae73b23f689229f4de6ac95d1f70780b901e409c56431f0ee4e010d5502a33eef1fe2fcdba19c5345eb160078d9dee2b47cb96edd4e6a0000000000000000000000007506c12a824d73d9b08564d5afc22c949434755e00000000000000000000000000000000000000000000000000000000000000e00000000000000000000000000000000000000000000000000000000065ec253b0000000000000000000000000000000000000000000000000000000065ec28bf66348c483727fb2eb578ccd1e2240d2113f20367fec28e95a9d613cdb516e2bb8d618511d2d85ca1cd13cfc65a40179be41b0cae91b81b1e8cfadaa15ca492bc00000000000000000000000000000000000000000000000000000000000000c4da2ff2f546bc2970c11ab5255dcf452cbd6a4da50547ff1f8cf7c8a5708810d52eb85fe20000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000004189ae420141c046a651b88b37c70cae3726cf48d8b561f4fc79feb8b910860f1e1ebdde770ef7ea5ce72493d2d2b6bc9a79b530c57fea7e591a4b1a197c7cce8a1c0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c001a0280d27bf5f0a7e00a125a742c0727ca18dda3ab2b1e1171133f1b6ccdf918f30a0722d537cade98c4799852077b3100c999bda5a9b1fa02fc3deea0bed0f1539de","0x02f8f583aa36a7821401843b9aca0084b2d05e00830156c29475a6b961c8da942ee03ca641b09c322549f6fa9880b8849aaab6483bfe8a55f85a2a91f45ddc40b097dfef45b2dd245cf347aec2a0bbac31bc163e00000000000000000000000000000000000000000000000000000000000e1168cf952fe6650f909d96c72da5bfa9056284a3ccd733bec61b3cad3e2c6fe3857c0000000000000000000000000000000000000000000000000000000000532180c001a07fbc2d9293870837043367909d42882002b33f54adabf25fe59c2d7432180e54a039eedc65f9ca9e16465b15314b43f11a3964d1ad799db1b0c7b5031eae5a252a","0x02f9011383aa36a78305b53a843b9aca0084b2d05e00825bbc94ff0000000000000000000000000000000004206980b8a200b45ef8dbdc47267ddffe8775fe84ff8e00000000008a78dadae1cff0c36781c7977ebb87ef238fb684a45cdeb5299d937169c40e569dde637ffb788fba24164635072b362ef0363f3fdbf47f4eed834737e7f389f22ffd1366e61593dda2d4bedc7011c781568d96d437aa46072006fef799b1a6b9f13f97e9bacceb3fdf983f4c39a7beb3605afe824f1382e29e5bb6b3106da0f90140000000ffff74e34f2b01c001a0ca432943bd2f4d3b5766b4a855dc137e9c4651ccfdf3221c1346006e495b3f0ba01448a7fbc3837feb3a7f8426899b752d7c8d6b5df78140fa356969dea950cd8d"],
                "withdrawals":[{"index":"0x257c870","validatorIndex":"0x1de","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c871","validatorIndex":"0x1df","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c872","validatorIndex":"0x1e0","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c873","validatorIndex":"0x1e1","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c874","validatorIndex":"0x1e2","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c875","validatorIndex":"0x1e3","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c876","validatorIndex":"0x1e4","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c877","validatorIndex":"0x1e5","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c878","validatorIndex":"0x1e6","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c879","validatorIndex":"0x1e7","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c87a","validatorIndex":"0x1e8","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c87b","validatorIndex":"0x1e9","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c87c","validatorIndex":"0x1ea","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c87d","validatorIndex":"0x1eb","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"},{"index":"0x257c87e","validatorIndex":"0x1ec","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x38c64"},{"index":"0x257c87f","validatorIndex":"0x1ed","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0x36784"}]
            },
            [],
            "0x705d84b4afc89d063429e34886b51a2d8844862acca72d9192ea5f1d0903dd21"
        ]
    })"_json;
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator engine_newPayloadV3: patch gasUsed regex", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    // gasUsed regex at commit 5849052: "^0x([1-9a-f]+[0-9a-f]{0,15})|0$" does not work for input "gasUsed":"0x0"
    // gasUsed regex patch: "^0x([1-9a-f]+[0-9a-f]*|0)$"
    auto request = R"({
        "jsonrpc":"2.0",
        "id":23,
        "method":"engine_newPayloadV3",
        "params":[
            {
                "parentHash":"0x89524ffba439f613e30cff04611bf1e8dca0ab013e3db80af09b5d687dff1201",
                "feeRecipient":"0x0f35b0753e261375c9a6cb44316b4bdc7e765509",
                "stateRoot":"0x3844a42d6aca69fac803e3c891e0841242908b8e55cab1561fd7d1a417fe9b80",
                "receiptsRoot":"0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421",
                "logsBloom":"0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                "prevRandao":"0xa6846c71318111439896fe1f6f00f4def93c0527889aa43aa20acfbeed586dda",
                "blockNumber":"0x5321b0",
                "gasLimit":"0x1c9c380",
                "gasUsed":"0x0",
                "timestamp":"0x65ec272c",
                "extraData":"0xd883010d0b846765746888676f312e32312e36856c696e7578",
                "baseFeePerGas":"0x817760c",
                "blobGasUsed":"0x0",
                "excessBlobGas":"0x4c80000",
                "blockHash":"0x9db7d47eb2a5d03c56b8f437c9c29bd6ebd9688107b519263401d56ccfc3a8df",
                "transactions":[],
                "withdrawals":[{"index":"0x257cb00","validatorIndex":"0x611","address":"0xf97e180c050e5ab072211ad2c213eb5aee4df134","amount":"0xec0"},{"index":"0x257cb01","validatorIndex":"0x612","address":"0xf97e180c050e5ab072211ad2c213eb5aee4df134","amount":"0xec0"},{"index":"0x257cb02","validatorIndex":"0x614","address":"0xf97e180c050e5ab072211ad2c213eb5aee4df134","amount":"0xec0"},{"index":"0x257cb03","validatorIndex":"0x622","address":"0x388ea662ef2c223ec0b047d41bf3c0f362142ad5","amount":"0xec0"},{"index":"0x257cb04","validatorIndex":"0x191","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb05","validatorIndex":"0x193","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb06","validatorIndex":"0x19b","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb07","validatorIndex":"0x19c","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb08","validatorIndex":"0x19d","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb09","validatorIndex":"0x19e","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb0a","validatorIndex":"0x1a3","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb0b","validatorIndex":"0x1a5","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xec0"},{"index":"0x257cb0c","validatorIndex":"0x1a7","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xb10"},{"index":"0x257cb0d","validatorIndex":"0x1ab","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xb10"},{"index":"0x257cb0e","validatorIndex":"0x1ba","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xb10"},{"index":"0x257cb0f","validatorIndex":"0x1c4","address":"0x25c4a76e7d118705e7ea2e9b7d8c59930d8acd3b","amount":"0xb10"}]
            },
            [],
            "0xc3e3a313cb5b0e746326f0958cc5cbd931c5ccf9f6dde99e3ab0bd5d2c05d92a"
        ]
    })"_json;
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator engine_forkchoiceUpdatedV3: null payloadAttributes param", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    // Payload attributes at commit 5849052 does NOT allow for null value
    auto request = R"({
        "jsonrpc":"2.0",
        "id":37,
        "method":"engine_forkchoiceUpdatedV3",
        "params":[
            {
                "headBlockHash":"0xb0433cd89f470c3b72275a28198a4bb5b31cb7095f81a230a20c1774d5b93557",
                "safeBlockHash":"0x636bfa7b1c7d804c97de4a4cc33239899cf0406ac7a128b3277342af9a2e00a4",
                "finalizedBlockHash":"0x636bfa7b1c7d804c97de4a4cc33239899cf0406ac7a128b3277342af9a2e00a4"
            },
            null
        ]
    })"_json;
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

TEST_CASE("JsonRpcValidator engine_forkchoiceUpdatedV3: missing payloadAttributes param", "[rpc][http][json_rpc_validator]") {
    JsonRpcValidator validator{create_validator_for_test()};

    auto request = R"({
        "jsonrpc":"2.0",
        "id":37,
        "method":"engine_forkchoiceUpdatedV3",
        "params":[
            {
                "headBlockHash":"0xb0433cd89f470c3b72275a28198a4bb5b31cb7095f81a230a20c1774d5b93557",
                "safeBlockHash":"0x636bfa7b1c7d804c97de4a4cc33239899cf0406ac7a128b3277342af9a2e00a4",
                "finalizedBlockHash":"0x636bfa7b1c7d804c97de4a4cc33239899cf0406ac7a128b3277342af9a2e00a4"
            }
        ]
    })"_json;
    JsonRpcValidationResult result = validator.validate(request);
    CHECK(result);
}

}  // namespace silkworm::rpc::json_rpc
