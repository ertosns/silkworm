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

#include <memory>
#include <vector>

#include <silkworm/core/chain/config.hpp>
#include <silkworm/infra/grpc/server/server.hpp>
#include <silkworm/interfaces/remote/ethbackend.grpc.pb.h>
#include <silkworm/node/backend/ethereum_backend.hpp>

namespace silkworm::ethbackend::grpc::server {

class BackEndServer : public virtual rpc::Server {
  public:
    BackEndServer(const rpc::ServerSettings& settings, const EthereumBackEnd& backend);

    BackEndServer(const BackEndServer&) = delete;
    BackEndServer& operator=(const BackEndServer&) = delete;

  protected:
    void register_async_services(::grpc::ServerBuilder& builder) override;
    void register_request_calls() override;

  private:
    static void setup_backend_calls(const EthereumBackEnd& backend);
    void register_backend_request_calls(agrpc::GrpcContext* grpc_context);

    //! The Ethereum full node service.
    const EthereumBackEnd& backend_;

    //! \warning The gRPC service must exist for the lifetime of the gRPC server it is registered on.
    remote::ETHBACKEND::AsyncService backend_async_service_;
};

}  // namespace silkworm::ethbackend::grpc::server
