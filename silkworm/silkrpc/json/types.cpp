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

#include "types.hpp"

#include <cstring>
#include <span>
#include <utility>

#include <boost/endian/conversion.hpp>
#include <intx/intx.hpp>

#include <silkworm/core/common/endian.hpp>
#include <silkworm/core/common/util.hpp>
#include <silkworm/silkrpc/common/log.hpp>
#include <silkworm/silkrpc/common/util.hpp>

namespace silkworm::rpc {

using evmc::literals::operator""_address;

void to_hex(std::span<char> hex_bytes, silkworm::ByteView bytes) {
    static const char* kHexDigits{"0123456789abcdef"};
    if (bytes.size() * 2 + 2 + 1 > hex_bytes.size()) {
        throw std::invalid_argument("to_hex: hex_bytes too small");
    }
    char* dest = hex_bytes.data();
    *dest++ = '0';
    *dest++ = 'x';
    for (const auto& b : bytes) {
        *dest++ = kHexDigits[b >> 4];    // Hi
        *dest++ = kHexDigits[b & 0x0f];  // Lo
    }
    *dest = '\0';
}

void to_hex_no_leading_zeros(std::span<char> hex_bytes, silkworm::ByteView bytes) {
    static const char* kHexDigits{"0123456789abcdef"};
    size_t len = bytes.length();
    if (len * 2 + 2 + 1 > hex_bytes.size()) {
        throw std::invalid_argument("to_hex_no_leading_zeros: hex_bytes too small");
    }
    char* dest = hex_bytes.data();
    *dest++ = '0';
    *dest++ = 'x';

    bool found_nonzero{false};
    for (size_t i{0}; i < len; ++i) {
        auto x{bytes[i]};
        char lo{kHexDigits[x & 0x0f]};
        char hi{kHexDigits[x >> 4]};
        if (!found_nonzero && hi != '0') {
            found_nonzero = true;
        }
        if (found_nonzero) {
            *dest++ = hi;
        }
        if (!found_nonzero && lo != '0') {
            found_nonzero = true;
        }
        if (found_nonzero || i == len - 1) {
            *dest++ = lo;
        }
    }
    *dest = '\0';
}

void to_quantity(std::span<char> quantity_hex_bytes, silkworm::ByteView bytes) {
    to_hex_no_leading_zeros(quantity_hex_bytes, bytes);
}

void to_quantity(std::span<char> quantity_hex_bytes, uint64_t number) {
    silkworm::Bytes number_bytes(8, '\0');
    silkworm::endian::store_big_u64(number_bytes.data(), number);
    to_hex_no_leading_zeros(quantity_hex_bytes, number_bytes);
}

void to_quantity(std::span<char> quantity_hex_bytes, intx::uint256 number) {
    if (number == 0) {
        quantity_hex_bytes[0] = '0';
        quantity_hex_bytes[1] = 'x';
        quantity_hex_bytes[2] = '0';
        quantity_hex_bytes[3] = '\0';
        return;
    }
    to_quantity(quantity_hex_bytes, silkworm::endian::to_big_compact(number));
}

std::string to_hex_no_leading_zeros(silkworm::ByteView bytes) {
    static const char* kHexDigits{"0123456789abcdef"};

    std::string out{};

    if (bytes.length() == 0) {
        out.reserve(1);
        out.push_back('0');
        return out;
    }

    out.reserve(2 * bytes.length());

    bool found_nonzero{false};
    for (size_t i{0}; i < bytes.length(); ++i) {
        uint8_t x{bytes[i]};
        char lo{kHexDigits[x & 0x0f]};
        char hi{kHexDigits[x >> 4]};
        if (!found_nonzero && hi != '0') {
            found_nonzero = true;
        }
        if (found_nonzero) {
            out.push_back(hi);
        }
        if (!found_nonzero && lo != '0') {
            found_nonzero = true;
        }
        if (found_nonzero || i == bytes.length() - 1) {
            out.push_back(lo);
        }
    }

    return out;
}

std::string to_hex_no_leading_zeros(uint64_t number) {
    silkworm::Bytes number_bytes(8, '\0');
    boost::endian::store_big_u64(&number_bytes[0], number);
    return to_hex_no_leading_zeros(number_bytes);
}

std::string to_quantity(silkworm::ByteView bytes) {
    return "0x" + to_hex_no_leading_zeros(bytes);
}

std::string to_quantity(uint64_t number) {
    return "0x" + to_hex_no_leading_zeros(number);
}

std::string to_quantity(intx::uint256 number) {
    if (number == 0) {
        return "0x0";
    }
    return to_quantity(silkworm::endian::to_big_compact(number));
}

}  // namespace silkworm::rpc

namespace evmc {

void to_json(nlohmann::json& json, const address& addr) {
    json = "0x" + silkworm::to_hex(addr);
}

void from_json(const nlohmann::json& json, address& addr) {
    const auto address_bytes = silkworm::from_hex(json.get<std::string>());
    addr = silkworm::to_evmc_address(address_bytes.value_or(silkworm::Bytes{}));
}

void to_json(nlohmann::json& json, const bytes32& b32) {
    json = "0x" + silkworm::to_hex(b32);
}

void from_json(const nlohmann::json& json, bytes32& b32) {
    const auto b32_bytes = silkworm::from_hex(json.get<std::string>());
    b32 = silkworm::to_bytes32(b32_bytes.value_or(silkworm::Bytes{}));
}

}  // namespace evmc

namespace intx {

void from_json(const nlohmann::json& json, uint256& ui256) {
    ui256 = intx::from_string<intx::uint256>(json.get<std::string>());
}

}  // namespace intx

namespace silkworm {

void from_json(const nlohmann::json& json, AccessListEntry& entry) {
    entry.account = json.at("address").get<evmc::address>();
    entry.storage_keys = json.at("storageKeys").get<std::vector<evmc::bytes32>>();
}

void to_json(nlohmann::json& json, const BlockHeader& header) {
    const auto block_number = rpc::to_quantity(header.number);
    json["number"] = block_number;
    json["hash"] = rpc::to_quantity(header.hash());
    json["parentHash"] = header.parent_hash;
    json["nonce"] = "0x" + silkworm::to_hex({header.nonce.data(), header.nonce.size()});
    json["sha3Uncles"] = header.ommers_hash;
    json["logsBloom"] = "0x" + silkworm::to_hex(silkworm::full_view(header.logs_bloom));
    json["transactionsRoot"] = header.transactions_root;
    json["stateRoot"] = header.state_root;
    json["receiptsRoot"] = header.receipts_root;
    json["miner"] = header.beneficiary;
    json["difficulty"] = rpc::to_quantity(silkworm::endian::to_big_compact(header.difficulty));
    json["extraData"] = "0x" + silkworm::to_hex(header.extra_data);
    json["mixHash"] = header.mix_hash;
    json["gasLimit"] = rpc::to_quantity(header.gas_limit);
    json["gasUsed"] = rpc::to_quantity(header.gas_used);
    json["timestamp"] = rpc::to_quantity(header.timestamp);
    if (header.base_fee_per_gas.has_value()) {
        json["baseFeePerGas"] = rpc::to_quantity(header.base_fee_per_gas.value_or(0));
    } else {
        json["baseFeePerGas"] = nullptr;
    }
    json["withdrawalsRoot"] = nullptr;  // waiting EIP-4895
}

void to_json(nlohmann::json& json, const AccessListEntry& access_list) {
    json["address"] = access_list.account;
    json["storageKeys"] = access_list.storage_keys;
}

void to_json(nlohmann::json& json, const Transaction& transaction) {
    if (!transaction.from) {
        (const_cast<Transaction&>(transaction)).recover_sender();
    }
    if (transaction.from) {
        json["from"] = transaction.from.value();
    }
    json["gas"] = rpc::to_quantity(transaction.gas_limit);
    auto ethash_hash{hash_of_transaction(transaction)};
    json["hash"] = silkworm::to_bytes32({ethash_hash.bytes, silkworm::kHashLength});
    json["input"] = "0x" + silkworm::to_hex(transaction.data);
    json["nonce"] = rpc::to_quantity(transaction.nonce);
    if (transaction.to) {
        json["to"] = transaction.to.value();
    } else {
        json["to"] = nullptr;
    }
    json["type"] = rpc::to_quantity(uint64_t(transaction.type));

    if (transaction.type == silkworm::TransactionType::kEip1559) {
        json["maxPriorityFeePerGas"] = rpc::to_quantity(transaction.max_priority_fee_per_gas);
        json["maxFeePerGas"] = rpc::to_quantity(transaction.max_fee_per_gas);
    }
    if (transaction.type != silkworm::TransactionType::kLegacy) {
        json["chainId"] = rpc::to_quantity(*transaction.chain_id);
        json["v"] = rpc::to_quantity(uint64_t(transaction.odd_y_parity));
        json["accessList"] = transaction.access_list;  // EIP2930
    } else if (transaction.chain_id) {
        json["chainId"] = rpc::to_quantity(*transaction.chain_id);
        json["v"] = rpc::to_quantity(silkworm::endian::to_big_compact(transaction.v()));
    } else {
        json["v"] = rpc::to_quantity(silkworm::endian::to_big_compact(transaction.v()));
    }
    json["value"] = rpc::to_quantity(transaction.value);
    json["r"] = rpc::to_quantity(silkworm::endian::to_big_compact(transaction.r));
    json["s"] = rpc::to_quantity(silkworm::endian::to_big_compact(transaction.s));
}

}  // namespace silkworm

namespace silkworm::rpc {

void to_json(nlohmann::json& json, const ChainTraffic& chain_traffic) {
    json["cumulativeGasUsed"] = to_quantity(chain_traffic.cumulative_gas_used);
    json["cumulativeTransactionsCount"] = to_quantity(chain_traffic.cumulative_transactions_count);
}

void to_json(nlohmann::json& json, const StageData& stage_data) {
    json["stage_name"] = stage_data.stage_name;
    json["block_number"] = stage_data.block_number;
}

void to_json(nlohmann::json& json, const SyncingData& syncing_data) {
    json["currentBlock"] = syncing_data.current_block;
    json["highestBlock"] = syncing_data.highest_block;
    json["stages"] = syncing_data.stages;
}

void to_json(nlohmann::json& json, const struct TxPoolStatusInfo& status_info) {
    json["queued"] = to_quantity(status_info.queued);
    json["pending"] = to_quantity(status_info.pending);
    json["baseFee"] = to_quantity(status_info.base_fee);
}

void to_json(nlohmann::json& json, const Rlp& rlp) {
    json = "0x" + silkworm::to_hex(rlp.buffer);
}

void to_json(nlohmann::json& json, const NodeInfoPorts& node_info_ports) {
    json["discovery"] = node_info_ports.discovery;
    json["listener"] = node_info_ports.listener;
}

void to_json(nlohmann::json& json, const NodeInfo& node_info) {
    json["id"] = node_info.id;
    json["name"] = node_info.name;
    json["enode"] = node_info.enode;
    json["enr"] = node_info.enr;
    json["listenAddr"] = node_info.listener_addr;
    json["ports"] = node_info.ports;
    json["ip"] = node_info.enode;
    json["protocols"] = nlohmann::json::parse(node_info.protocols, nullptr, /* allow_exceptions = */ false);
}

void to_json(nlohmann::json& json, const PeerInfo& info) {
    json["id"] = info.id;
    json["name"] = info.name;
    json["enode"] = info.enode;
    if (!info.enr.empty()) {
        json["enr"] = info.enr;
    }
    json["caps"] = info.caps;
    json["network"]["localAddress"] = info.local_address;
    json["network"]["remoteAddress"] = info.remote_address;
    json["network"]["inbound"] = info.is_connection_inbound;
    json["network"]["static"] = info.is_connection_static;
    json["network"]["trusted"] = info.is_connection_trusted;
    json["protocols"] = nullptr;
}

void to_json(nlohmann::json& json, const struct CallBundleTxInfo& tx_info) {
    json["gasUsed"] = tx_info.gas_used;
    json["txHash"] = silkworm::to_bytes32({tx_info.hash.bytes, silkworm::kHashLength});
    if (!tx_info.error_message.empty())
        json["error"] = tx_info.error_message;
    else
        json["value"] = silkworm::to_bytes32({tx_info.value.bytes, silkworm::kHashLength});
}

void to_json(nlohmann::json& json, const struct CallBundleInfo& bundle_info) {
    json["bundleHash"] = silkworm::to_bytes32({bundle_info.bundle_hash.bytes, silkworm::kHashLength});
    json["results"] = bundle_info.txs_info;
}

void to_json(nlohmann::json& json, const AccessListResult& access_list_result) {
    json["accessList"] = access_list_result.access_list;
    if (access_list_result.error) {
        json["error"] = *(access_list_result.error);
    }
    json["gasUsed"] = to_quantity(access_list_result.gas_used);
}

void to_json(nlohmann::json& json, const Block& b) {
    const auto block_number = to_quantity(b.block.header.number);
    json["number"] = block_number;
    json["hash"] = b.hash;
    json["parentHash"] = b.block.header.parent_hash;
    json["nonce"] = "0x" + silkworm::to_hex({b.block.header.nonce.data(), b.block.header.nonce.size()});
    json["sha3Uncles"] = b.block.header.ommers_hash;
    json["logsBloom"] = "0x" + silkworm::to_hex(full_view(b.block.header.logs_bloom));
    json["transactionsRoot"] = b.block.header.transactions_root;
    json["stateRoot"] = b.block.header.state_root;
    json["receiptsRoot"] = b.block.header.receipts_root;
    json["miner"] = b.block.header.beneficiary;
    json["difficulty"] = to_quantity(silkworm::endian::to_big_compact(b.block.header.difficulty));
    json["totalDifficulty"] = to_quantity(silkworm::endian::to_big_compact(b.total_difficulty));
    json["extraData"] = "0x" + silkworm::to_hex(b.block.header.extra_data);
    json["mixHash"] = b.block.header.mix_hash;
    json["size"] = to_quantity(b.get_block_size());
    json["gasLimit"] = to_quantity(b.block.header.gas_limit);
    json["gasUsed"] = to_quantity(b.block.header.gas_used);
    if (b.block.header.base_fee_per_gas.has_value()) {
        json["baseFeePerGas"] = to_quantity(b.block.header.base_fee_per_gas.value_or(0));
    }
    json["timestamp"] = to_quantity(b.block.header.timestamp);
    if (b.full_tx) {
        json["transactions"] = b.block.transactions;
        for (std::size_t i{0}; i < json["transactions"].size(); i++) {
            auto& json_txn = json["transactions"][i];
            json_txn["transactionIndex"] = to_quantity(i);
            json_txn["blockHash"] = b.hash;
            json_txn["blockNumber"] = block_number;
            json_txn["gasPrice"] = to_quantity(b.block.transactions[i].effective_gas_price(b.block.header.base_fee_per_gas.value_or(0)));
        }
    } else {
        std::vector<evmc::bytes32> transaction_hashes;
        transaction_hashes.reserve(b.block.transactions.size());
        for (std::size_t i{0}; i < b.block.transactions.size(); i++) {
            auto ethash_hash{hash_of_transaction(b.block.transactions[i])};
            auto bytes32_hash = silkworm::to_bytes32({ethash_hash.bytes, silkworm::kHashLength});
            transaction_hashes.emplace(transaction_hashes.end(), bytes32_hash);
            SILKRPC_DEBUG << "transaction_hashes[" << i << "]: " << silkworm::to_hex({transaction_hashes[i].bytes, silkworm::kHashLength}) << "\n";
        }
        json["transactions"] = transaction_hashes;
    }
    std::vector<evmc::bytes32> ommer_hashes;
    ommer_hashes.reserve(b.block.ommers.size());
    for (std::size_t i{0}; i < b.block.ommers.size(); i++) {
        ommer_hashes.emplace(ommer_hashes.end(), b.block.ommers[i].hash());
        SILKRPC_DEBUG << "ommer_hashes[" << i << "]: " << silkworm::to_hex({ommer_hashes[i].bytes, silkworm::kHashLength}) << "\n";
    }
    json["uncles"] = ommer_hashes;
}

void to_json(nlohmann::json& json, const BlockDetailsResponse& b) {
    const auto block_number = to_quantity(b.block.header.number);
    json["block"]["number"] = block_number;
    json["block"]["difficulty"] = to_quantity(silkworm::endian::to_big_compact(b.block.header.difficulty));
    json["block"]["extraData"] = "0x" + silkworm::to_hex(b.block.header.extra_data);
    json["block"]["gasLimit"] = to_quantity(b.block.header.gas_limit);
    json["block"]["gasUsed"] = to_quantity(b.block.header.gas_used);
    json["block"]["hash"] = b.block.hash;
    json["block"]["logsBloom"] = nullptr;
    json["block"]["miner"] = b.block.header.beneficiary;
    json["block"]["mixHash"] = b.block.header.mix_hash;
    json["block"]["nonce"] = "0x" + silkworm::to_hex({b.block.header.nonce.data(), b.block.header.nonce.size()});
    json["block"]["parentHash"] = b.block.header.parent_hash;
    json["block"]["receiptsRoot"] = b.block.header.receipts_root;
    json["block"]["sha3Uncles"] = b.block.header.ommers_hash;
    json["block"]["size"] = to_quantity(b.block.block_size);
    json["block"]["stateRoot"] = b.block.header.state_root;
    json["block"]["timestamp"] = to_quantity(b.block.header.timestamp);
    json["block"]["totalDifficulty"] = to_quantity(silkworm::endian::to_big_compact(b.block.total_difficulty));
    json["block"]["transactionCount"] = b.block.transaction_count;  // to_quantity(b.block.transaction_count);
    json["block"]["transactionsRoot"] = b.block.header.transactions_root;

    std::vector<evmc::bytes32> ommer_hashes;
    ommer_hashes.reserve(b.block.ommers.size());
    for (std::size_t i{0}; i < b.block.ommers.size(); i++) {
        ommer_hashes.emplace(ommer_hashes.end(), b.block.ommers[i].hash());
        SILKRPC_DEBUG << "ommer_hashes[" << i << "]: " << silkworm::to_hex({ommer_hashes[i].bytes, silkworm::kHashLength}) << "\n";
    }
    json["block"]["uncles"] = ommer_hashes;

    if (b.issuance.total_reward > 0) {
        json["issuance"]["minerReward"] = to_quantity(b.issuance.miner_reward);
        json["issuance"]["ommersReward"] = to_quantity(b.issuance.ommers_reward);
        json["issuance"]["totalReward"] = to_quantity(b.issuance.total_reward);
    } else {
        json["issuance"] = nlohmann::json::object();
    }

    json["totalFees"] = to_quantity(b.total_fees);
}

void to_json(nlohmann::json& json, const BlockTransactionsResponse& b) {
    const auto block_number = to_quantity(b.header.number);
    json["fullblock"]["difficulty"] = to_quantity(silkworm::endian::to_big_compact(b.header.difficulty));
    json["fullblock"]["extraData"] = "0x" + silkworm::to_hex(b.header.extra_data);
    json["fullblock"]["gasLimit"] = to_quantity(b.header.gas_limit);
    json["fullblock"]["gasUsed"] = to_quantity(b.header.gas_used);
    json["fullblock"]["hash"] = b.hash;
    json["fullblock"]["logsBloom"];
    json["fullblock"]["miner"] = b.header.beneficiary;
    json["fullblock"]["mixHash"] = b.header.mix_hash;
    json["fullblock"]["nonce"] = "0x" + silkworm::to_hex({b.header.nonce.data(), b.header.nonce.size()});
    json["fullblock"]["number"] = block_number;
    json["fullblock"]["parentHash"] = b.header.parent_hash;
    json["fullblock"]["receiptsRoot"] = b.header.receipts_root;
    json["fullblock"]["sha3Uncles"] = b.header.ommers_hash;
    json["fullblock"]["size"] = to_quantity(b.block_size);
    json["fullblock"]["stateRoot"] = b.header.state_root;
    json["fullblock"]["timestamp"] = to_quantity(b.header.timestamp);
    json["fullblock"]["totalDifficulty"] = to_quantity(silkworm::endian::to_big_compact(b.total_difficulty));
    json["fullblock"]["transactionCount"] = b.transaction_count;

    json["fullblock"]["transactions"] = b.transactions;
    for (std::size_t i{0}; i < json["fullblock"]["transactions"].size(); i++) {
        auto& json_txn = json["fullblock"]["transactions"][i];
        json_txn["transactionIndex"] = to_quantity(b.receipts.at(i).tx_index);
        json_txn["blockHash"] = b.hash;
        json_txn["blockNumber"] = block_number;
        json_txn["gasPrice"] = to_quantity(b.transactions[i].effective_gas_price(b.header.base_fee_per_gas.value_or(0)));
        json_txn["input"] = "0x" + silkworm::to_hex(b.transactions[i].data.substr(0, 4));
    }

    json["fullblock"]["transactionsRoot"] = b.header.transactions_root;

    std::vector<evmc::bytes32> ommer_hashes;
    ommer_hashes.reserve(b.ommers.size());
    for (std::size_t i{0}; i < b.ommers.size(); i++) {
        ommer_hashes.emplace(ommer_hashes.end(), b.ommers[i].hash());
        SILKRPC_DEBUG << "ommer_hashes[" << i << "]: " << silkworm::to_hex({ommer_hashes[i].bytes, silkworm::kHashLength}) << "\n";
    }

    json["fullblock"]["uncles"] = ommer_hashes;
    json["receipts"] = b.receipts;
    for (std::size_t i{0}; i < json["receipts"].size(); i++) {
        auto& json_txn = json["receipts"][i];
        json_txn["logs"] = nullptr;
        json_txn["logsBloom"] = nullptr;
        json_txn["effectiveGasPrice"] = to_quantity(b.transactions[i].effective_gas_price(b.header.base_fee_per_gas.value_or(0)));
    }
}

void to_json(nlohmann::json& json, const Transaction& transaction) {
    to_json(json, static_cast<const silkworm::Transaction&>(transaction));

    json["gasPrice"] = to_quantity(transaction.effective_gas_price());
    if (transaction.queued_in_pool) {
        json["blockHash"] = nullptr;
        json["blockNumber"] = nullptr;
        json["transactionIndex"] = nullptr;
    } else {
        json["blockHash"] = transaction.block_hash;
        json["blockNumber"] = to_quantity(transaction.block_number);
        json["transactionIndex"] = to_quantity(transaction.transaction_index);
    }
}

void to_json(nlohmann::json& json, const ForkChoiceState& forkchoice_state) {
    json["headBlockHash"] = forkchoice_state.head_block_hash;
    json["safeBlockHash"] = forkchoice_state.safe_block_hash;
    json["finalizedBlockHash"] = forkchoice_state.finalized_block_hash;
}

void from_json(const nlohmann::json& json, ForkChoiceState& forkchoice_state) {
    forkchoice_state = ForkChoiceState{
        .head_block_hash = json.at("headBlockHash").get<evmc::bytes32>(),
        .safe_block_hash = json.at("safeBlockHash").get<evmc::bytes32>(),
        .finalized_block_hash = json.at("finalizedBlockHash").get<evmc::bytes32>()};
}

void to_json(nlohmann::json& json, const ForkChoiceUpdatedReply& forkchoice_updated_reply) {
    nlohmann::json json_payload_status = forkchoice_updated_reply.payload_status;
    json["payloadStatus"] = json_payload_status;
    if (forkchoice_updated_reply.payload_id != std::nullopt) {
        json["payloadId"] = to_quantity(forkchoice_updated_reply.payload_id.value());
    }
}

void to_json(nlohmann::json& json, const PayloadStatus& payload_status) {
    json["status"] = payload_status.status;

    if (payload_status.latest_valid_hash) {
        json["latestValidHash"] = *payload_status.latest_valid_hash;
    }
    if (payload_status.validation_error) {
        json["validationError"] = *payload_status.validation_error;
    }
}

void to_json(nlohmann::json& json, const TransitionConfiguration& transition_configuration) {
    json["terminalTotalDifficulty"] = to_quantity(transition_configuration.terminal_total_difficulty);
    json["terminalBlockHash"] = transition_configuration.terminal_block_hash;
    json["terminalBlockNumber"] = to_quantity(transition_configuration.terminal_block_number);
}

void from_json(const nlohmann::json& json, TransitionConfiguration& transition_configuration) {
    transition_configuration = TransitionConfiguration{
        .terminal_total_difficulty = json.at("terminalTotalDifficulty").get<intx::uint256>(),
        .terminal_block_hash = json.at("terminalBlockHash").get<evmc::bytes32>(),
        .terminal_block_number = static_cast<uint64_t>(std::stol(json.at("terminalBlockNumber").get<std::string>(), nullptr, 16))};
}

void to_json(nlohmann::json& json, const Forks& forks) {
    json["genesis"] = forks.genesis_hash;
    json["heightForks"] = forks.block_numbers;
    json["timeForks"] = forks.block_times;
}

void to_json(nlohmann::json& json, const Issuance& issuance) {
    if (issuance.block_reward) {
        json["blockReward"] = issuance.block_reward.value();
    } else {
        json["blockReward"] = nullptr;
    }
    if (issuance.ommer_reward) {
        json["uncleReward"] = issuance.ommer_reward.value();
    } else {
        json["uncleReward"] = nullptr;
    }
    if (issuance.issuance) {
        json["issuance"] = issuance.issuance.value();
    } else {
        json["issuance"] = nullptr;
    }
    if (issuance.burnt) {
        json["burnt"] = issuance.burnt.value();
    } else {
        json["burnt"] = nullptr;
    }
    if (issuance.total_issued) {
        json["totalIssued"] = issuance.total_issued.value();
    } else {
        json["totalIssued"] = nullptr;
    }
    if (issuance.total_burnt) {
        json["totalBurnt"] = issuance.total_burnt.value();
    } else {
        json["totalBurnt"] = nullptr;
    }
    if (issuance.tips) {
        json["tips"] = issuance.tips.value();
    } else {
        json["tips"] = nullptr;
    }
}

void to_json(nlohmann::json& json, const Error& error) {
    json = {{"code", error.code}, {"message", error.message}};
}

void to_json(nlohmann::json& json, const RevertError& error) {
    json = {{"code", error.code}, {"message", error.message}, {"data", "0x" + silkworm::to_hex(error.data)}};
}

void to_json(nlohmann::json& json, const std::set<evmc::address>& addresses) {
    json = nlohmann::json::array();
    for (const auto& address : addresses) {
        json.push_back("0x" + silkworm::to_hex(address));
    }
}

nlohmann::json make_json_content(uint32_t id) {
    return {{"jsonrpc", "2.0"}, {"id", id}, {"result", nullptr}};
}

nlohmann::json make_json_content(uint32_t id, const nlohmann::json& result) {
    return {{"jsonrpc", "2.0"}, {"id", id}, {"result", result}};
}

nlohmann::json make_json_error(uint32_t id, int64_t code, const std::string& message) {
    const Error error{code, message};
    return {{"jsonrpc", "2.0"}, {"id", id}, {"error", error}};
}

nlohmann::json make_json_error(uint32_t id, const RevertError& error) {
    return {{"jsonrpc", "2.0"}, {"id", id}, {"error", error}};
}

static constexpr auto errorMessageSize = 1024;
struct GlazeJsonError {
    int code;
    char message[errorMessageSize];
    struct glaze {
        using T = GlazeJsonError;
        static constexpr auto value = glz::object(
            "code", &T::code,
            "message", &T::message);
    };
};

struct GlazeJsonErrorRsp {
    char jsonrpc[jsonVersionSize] = "2.0";
    uint32_t id;
    GlazeJsonError json_error;
    struct glaze {
        using T = GlazeJsonErrorRsp;
        static constexpr auto value = glz::object(
            "jsonrpc", &T::jsonrpc,
            "id", &T::id,
            "error", &T::json_error);
    };
};

void make_glaze_json_error(std::string& reply, uint32_t id, const int code, const std::string& message) {
    GlazeJsonErrorRsp glaze_json_error;
    glaze_json_error.id = id;
    glaze_json_error.json_error.code = code;
    std::strncpy(glaze_json_error.json_error.message, message.c_str(), message.size() > errorMessageSize ? errorMessageSize : message.size() + 1);
    glz::write_json(glaze_json_error, reply);
}

struct GlazeJsonRevert {
    int code;
    char message[errorMessageSize];
    std::string data;
    struct glaze {
        using T = GlazeJsonRevert;
        static constexpr auto value = glz::object(
            "code", &T::code,
            "message", &T::message,
            "data", &T::data);
    };
};

struct GlazeJsonRevertError {
    char jsonrpc[jsonVersionSize] = "2.0";
    uint32_t id;
    GlazeJsonRevert revert_data;
    struct glaze {
        using T = GlazeJsonRevertError;
        static constexpr auto value = glz::object(
            "jsonrpc", &T::jsonrpc,
            "id", &T::id,
            "error", &T::revert_data);
    };
};

void make_glaze_json_error(std::string& reply, uint32_t id, const RevertError& error) {
    GlazeJsonRevertError glaze_json_revert;
    glaze_json_revert.id = id;
    glaze_json_revert.revert_data.code = error.code;
    std::strncpy(glaze_json_revert.revert_data.message, error.message.c_str(), error.message.size() > errorMessageSize ? errorMessageSize : error.message.size() + 1);
    glaze_json_revert.revert_data.data = "0x" + silkworm::to_hex(error.data);
    glz::write_json(glaze_json_revert, reply);
}

}  // namespace silkworm::rpc
