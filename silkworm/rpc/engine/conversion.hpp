/*
   Copyright 2024 The Silkworm Authors

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

#include <silkworm/node/execution/api/endpoint/checkers.hpp>
#include <silkworm/node/execution/api/endpoint/range.hpp>
#include <silkworm/rpc/types/execution_payload.hpp>

namespace silkworm::rpc::engine {

rpc::ForkChoiceUpdatedReply fork_choice_updated_reply_from_result(const execution::api::ForkChoiceResult&);

rpc::ExecutionPayloadBodies execution_payloads_from_bodies(const execution::api::BlockBodies&);

//! Convert an ExecutionPayload to a Block as per Engine API spec
std::shared_ptr<Block> block_from_execution_payload(const rpc::ExecutionPayload&);

}  // namespace silkworm::rpc::engine
