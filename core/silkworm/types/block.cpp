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

#include "block.hpp"

#include <silkworm/common/cast.hpp>
#include <silkworm/rlp/encode_vector.hpp>

namespace silkworm {

evmc::bytes32 BlockHeader::hash(bool for_sealing, bool exclude_extra_data_sig) const {
    Bytes rlp;
    rlp::encode(rlp, *this, for_sealing, exclude_extra_data_sig);
    return bit_cast<evmc_bytes32>(keccak256(rlp));
}

ethash::hash256 BlockHeader::boundary() const {
    using intx::operator""_u256;
    static const auto dividend{intx::uint320{1} << 256};
    auto result{difficulty > 1u ? intx::uint256{dividend / difficulty}
                                : 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff_u256};
    return intx::be::store<ethash::hash256>(result);
}

//! \brief Recover transaction senders for each block.
void Block::recover_senders() {
    for (Transaction& txn : transactions) {
        txn.recover_sender();
    }
}

namespace rlp {

    static Header rlp_header(const BlockHeader& header, bool for_sealing = false, bool exclude_extra_data_sig = false) {
        Header rlp_head{.list = true};
        rlp_head.payload_length += kHashLength + 1;                                        // parent_hash
        rlp_head.payload_length += kHashLength + 1;                                        // ommers_hash
        rlp_head.payload_length += kAddressLength + 1;                                     // beneficiary
        rlp_head.payload_length += kHashLength + 1;                                        // state_root
        rlp_head.payload_length += kHashLength + 1;                                        // transactions_root
        rlp_head.payload_length += kHashLength + 1;                                        // receipts_root
        rlp_head.payload_length += kBloomByteLength + length_of_length(kBloomByteLength);  // logs_bloom
        rlp_head.payload_length += length(header.difficulty);                              // difficulty
        rlp_head.payload_length += length(header.number);                                  // block height
        rlp_head.payload_length += length(header.gas_limit);                               // gas_limit
        rlp_head.payload_length += length(header.gas_used);                                // gas_used
        rlp_head.payload_length += length(header.timestamp);                               // timestamp
        if (exclude_extra_data_sig) {
            const auto extra_data_no_signature = header.extra_data.substr(0, header.extra_data.length() - kExtraSealSize);
            rlp_head.payload_length += length(extra_data_no_signature);  // extra_data -signature
        } else {
            rlp_head.payload_length += length(header.extra_data);  // extra_data
        }
        if (!for_sealing) {
            rlp_head.payload_length += kHashLength + 1;  // mix_hash
            rlp_head.payload_length += 8 + 1;            // nonce
        }
        if (header.base_fee_per_gas) {
            rlp_head.payload_length += length(*header.base_fee_per_gas);
        }
        if (header.withdrawals_root) {
            rlp_head.payload_length += kHashLength + 1;
        }
        return rlp_head;
    }

    size_t length(const BlockHeader& header) {
        const Header rlp_head{rlp_header(header)};
        return length_of_length(rlp_head.payload_length) + rlp_head.payload_length;
    }

    void encode(Bytes& to, const BlockHeader& header, bool for_sealing, bool exclude_extra_data_sig) {
        encode_header(to, rlp_header(header, for_sealing, exclude_extra_data_sig));
        encode(to, header.parent_hash.bytes);
        encode(to, header.ommers_hash.bytes);
        encode(to, header.beneficiary.bytes);
        encode(to, header.state_root.bytes);
        encode(to, header.transactions_root.bytes);
        encode(to, header.receipts_root.bytes);
        encode(to, header.logs_bloom);
        encode(to, header.difficulty);
        encode(to, header.number);
        encode(to, header.gas_limit);
        encode(to, header.gas_used);
        encode(to, header.timestamp);
        if (exclude_extra_data_sig) {
            const auto extra_data_no_signature = header.extra_data.substr(0, header.extra_data.length() - kExtraSealSize);
            encode(to, extra_data_no_signature);
        } else {
            encode(to, header.extra_data);
        }
        if (!for_sealing) {
            encode(to, header.mix_hash.bytes);
            encode(to, header.nonce);
        }
        if (header.base_fee_per_gas) {
            encode(to, *header.base_fee_per_gas);
        }
        if (header.withdrawals_root) {
            encode(to, *header.withdrawals_root);
        }
    }

    template <>
    DecodingResult decode(ByteView& from, BlockHeader& to) noexcept {
        auto [rlp_head, err1]{decode_header(from)};
        if (err1 != DecodingResult::kOk) {
            return err1;
        }
        if (!rlp_head.list) {
            return DecodingResult::kUnexpectedString;
        }
        uint64_t leftover{from.length() - rlp_head.payload_length};

        if (DecodingResult err{decode(from, to.parent_hash.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.ommers_hash.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.beneficiary.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.state_root.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.transactions_root.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.receipts_root.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.logs_bloom)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.difficulty)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.number)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.gas_limit)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.gas_used)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.timestamp)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.extra_data)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.mix_hash.bytes)}; err != DecodingResult::kOk) {
            return err;
        }
        if (DecodingResult err{decode(from, to.nonce)}; err != DecodingResult::kOk) {
            return err;
        }

        to.base_fee_per_gas = std::nullopt;
        if (from.length() > leftover) {
            intx::uint256 base_fee_per_gas;
            if (DecodingResult err{decode(from, base_fee_per_gas)}; err != DecodingResult::kOk) {
                return err;
            }
            to.base_fee_per_gas = base_fee_per_gas;
        }

        to.withdrawals_root = std::nullopt;
        if (from.length() > leftover) {
            evmc::bytes32 withdrawals_root;
            if (DecodingResult err{decode(from, withdrawals_root)}; err != DecodingResult::kOk) {
                return err;
            }
            to.withdrawals_root = withdrawals_root;
        }

        return from.length() == leftover ? DecodingResult::kOk : DecodingResult::kListLengthMismatch;
    }

    static Header rlp_header_body(const BlockBody& b) {
        Header rlp_head{.list = true};
        rlp_head.payload_length += length(b.transactions);
        rlp_head.payload_length += length(b.ommers);
        if (b.withdrawals) {
            rlp_head.payload_length += length(*b.withdrawals);
        }
        return rlp_head;
    }

    size_t length(const BlockBody& block_body) {
        const Header rlp_head{rlp_header_body(block_body)};
        return length_of_length(rlp_head.payload_length) + rlp_head.payload_length;
    }

    void encode(Bytes& to, const BlockBody& block_body) {
        encode_header(to, rlp_header_body(block_body));
        encode(to, block_body.transactions);
        encode(to, block_body.ommers);
        if (block_body.withdrawals) {
            encode(to, *block_body.withdrawals);
        }
    }

    template <>
    DecodingResult decode(ByteView& from, BlockBody& to) noexcept {
        auto [rlp_head, err]{decode_header(from)};
        if (err != DecodingResult::kOk) {
            return err;
        }
        if (!rlp_head.list) {
            return DecodingResult::kUnexpectedString;
        }
        uint64_t leftover{from.length() - rlp_head.payload_length};

        if (err = decode(from, to.transactions); err != DecodingResult::kOk) {
            return err;
        }
        if (err = decode(from, to.ommers); err != DecodingResult::kOk) {
            return err;
        }

        to.withdrawals = std::nullopt;
        if (from.length() > leftover) {
            std::vector<Withdrawal> withdrawals;
            if (err = decode(from, withdrawals); err != DecodingResult::kOk) {
                return err;
            }
            to.withdrawals = withdrawals;
        }

        return from.length() == leftover ? DecodingResult::kOk : DecodingResult::kListLengthMismatch;
    }

    template <>
    DecodingResult decode(ByteView& from, Block& to) noexcept {
        auto [rlp_head, err]{decode_header(from)};
        if (err != DecodingResult::kOk) {
            return err;
        }
        if (!rlp_head.list) {
            return DecodingResult::kUnexpectedString;
        }
        uint64_t leftover{from.length() - rlp_head.payload_length};

        if (err = decode(from, to.header); err != DecodingResult::kOk) {
            return err;
        }
        if (err = decode(from, to.transactions); err != DecodingResult::kOk) {
            return err;
        }
        if (err = decode(from, to.ommers); err != DecodingResult::kOk) {
            return err;
        }

        to.withdrawals = std::nullopt;
        if (from.length() > leftover) {
            std::vector<Withdrawal> withdrawals;
            if (err = decode(from, withdrawals); err != DecodingResult::kOk) {
                return err;
            }
            to.withdrawals = withdrawals;
        }

        return from.length() == leftover ? DecodingResult::kOk : DecodingResult::kListLengthMismatch;
    }

    static Header rlp_header(const Block& b) {
        Header rlp_head{rlp_header_body(b)};
        rlp_head.payload_length += length(b.header);
        return rlp_head;
    }

    size_t length(const Block& block) {
        const Header rlp_head{rlp_header(block)};
        return length_of_length(rlp_head.payload_length) + rlp_head.payload_length;
    }

    void encode(Bytes& to, const Block& block) {
        encode_header(to, rlp_header(block));
        encode(to, block.header);
        encode(to, block.transactions);
        encode(to, block.ommers);
        if (block.withdrawals) {
            encode(to, *block.withdrawals);
        }
    }

}  // namespace rlp
}  // namespace silkworm
