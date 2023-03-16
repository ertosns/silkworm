// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: txpool/txpool.proto

#include "txpool/txpool.pb.h"
#include "txpool/txpool.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace txpool {

static const char* Txpool_method_names[] = {
  "/txpool.Txpool/Version",
  "/txpool.Txpool/FindUnknown",
  "/txpool.Txpool/Add",
  "/txpool.Txpool/Transactions",
  "/txpool.Txpool/All",
  "/txpool.Txpool/Pending",
  "/txpool.Txpool/OnAdd",
  "/txpool.Txpool/Status",
  "/txpool.Txpool/Nonce",
};

std::unique_ptr< Txpool::Stub> Txpool::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Txpool::Stub> stub(new Txpool::Stub(channel, options));
  return stub;
}

Txpool::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_Version_(Txpool_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_FindUnknown_(Txpool_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Add_(Txpool_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Transactions_(Txpool_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_All_(Txpool_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Pending_(Txpool_method_names[5], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_OnAdd_(Txpool_method_names[6], options.suffix_for_stats(),::grpc::internal::RpcMethod::SERVER_STREAMING, channel)
  , rpcmethod_Status_(Txpool_method_names[7], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_Nonce_(Txpool_method_names[8], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Txpool::Stub::Version(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::types::VersionReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::google::protobuf::Empty, ::types::VersionReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Version_, context, request, response);
}

void Txpool::Stub::async::Version(::grpc::ClientContext* context, const ::google::protobuf::Empty* request, ::types::VersionReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::google::protobuf::Empty, ::types::VersionReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Version_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Version(::grpc::ClientContext* context, const ::google::protobuf::Empty* request, ::types::VersionReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Version_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::types::VersionReply>* Txpool::Stub::PrepareAsyncVersionRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::types::VersionReply, ::google::protobuf::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Version_, context, request);
}

::grpc::ClientAsyncResponseReader< ::types::VersionReply>* Txpool::Stub::AsyncVersionRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncVersionRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::FindUnknown(::grpc::ClientContext* context, const ::txpool::TxHashes& request, ::txpool::TxHashes* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::TxHashes, ::txpool::TxHashes, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_FindUnknown_, context, request, response);
}

void Txpool::Stub::async::FindUnknown(::grpc::ClientContext* context, const ::txpool::TxHashes* request, ::txpool::TxHashes* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::TxHashes, ::txpool::TxHashes, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_FindUnknown_, context, request, response, std::move(f));
}

void Txpool::Stub::async::FindUnknown(::grpc::ClientContext* context, const ::txpool::TxHashes* request, ::txpool::TxHashes* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_FindUnknown_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::TxHashes>* Txpool::Stub::PrepareAsyncFindUnknownRaw(::grpc::ClientContext* context, const ::txpool::TxHashes& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::TxHashes, ::txpool::TxHashes, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_FindUnknown_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::TxHashes>* Txpool::Stub::AsyncFindUnknownRaw(::grpc::ClientContext* context, const ::txpool::TxHashes& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncFindUnknownRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::Add(::grpc::ClientContext* context, const ::txpool::AddRequest& request, ::txpool::AddReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::AddRequest, ::txpool::AddReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Add_, context, request, response);
}

void Txpool::Stub::async::Add(::grpc::ClientContext* context, const ::txpool::AddRequest* request, ::txpool::AddReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::AddRequest, ::txpool::AddReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Add_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Add(::grpc::ClientContext* context, const ::txpool::AddRequest* request, ::txpool::AddReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Add_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::AddReply>* Txpool::Stub::PrepareAsyncAddRaw(::grpc::ClientContext* context, const ::txpool::AddRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::AddReply, ::txpool::AddRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Add_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::AddReply>* Txpool::Stub::AsyncAddRaw(::grpc::ClientContext* context, const ::txpool::AddRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncAddRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::Transactions(::grpc::ClientContext* context, const ::txpool::TransactionsRequest& request, ::txpool::TransactionsReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::TransactionsRequest, ::txpool::TransactionsReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Transactions_, context, request, response);
}

void Txpool::Stub::async::Transactions(::grpc::ClientContext* context, const ::txpool::TransactionsRequest* request, ::txpool::TransactionsReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::TransactionsRequest, ::txpool::TransactionsReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Transactions_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Transactions(::grpc::ClientContext* context, const ::txpool::TransactionsRequest* request, ::txpool::TransactionsReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Transactions_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::TransactionsReply>* Txpool::Stub::PrepareAsyncTransactionsRaw(::grpc::ClientContext* context, const ::txpool::TransactionsRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::TransactionsReply, ::txpool::TransactionsRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Transactions_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::TransactionsReply>* Txpool::Stub::AsyncTransactionsRaw(::grpc::ClientContext* context, const ::txpool::TransactionsRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncTransactionsRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::All(::grpc::ClientContext* context, const ::txpool::AllRequest& request, ::txpool::AllReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::AllRequest, ::txpool::AllReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_All_, context, request, response);
}

void Txpool::Stub::async::All(::grpc::ClientContext* context, const ::txpool::AllRequest* request, ::txpool::AllReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::AllRequest, ::txpool::AllReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_All_, context, request, response, std::move(f));
}

void Txpool::Stub::async::All(::grpc::ClientContext* context, const ::txpool::AllRequest* request, ::txpool::AllReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_All_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::AllReply>* Txpool::Stub::PrepareAsyncAllRaw(::grpc::ClientContext* context, const ::txpool::AllRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::AllReply, ::txpool::AllRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_All_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::AllReply>* Txpool::Stub::AsyncAllRaw(::grpc::ClientContext* context, const ::txpool::AllRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncAllRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::Pending(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::txpool::PendingReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::google::protobuf::Empty, ::txpool::PendingReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Pending_, context, request, response);
}

void Txpool::Stub::async::Pending(::grpc::ClientContext* context, const ::google::protobuf::Empty* request, ::txpool::PendingReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::google::protobuf::Empty, ::txpool::PendingReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Pending_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Pending(::grpc::ClientContext* context, const ::google::protobuf::Empty* request, ::txpool::PendingReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Pending_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::PendingReply>* Txpool::Stub::PrepareAsyncPendingRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::PendingReply, ::google::protobuf::Empty, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Pending_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::PendingReply>* Txpool::Stub::AsyncPendingRaw(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncPendingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::ClientReader< ::txpool::OnAddReply>* Txpool::Stub::OnAddRaw(::grpc::ClientContext* context, const ::txpool::OnAddRequest& request) {
  return ::grpc::internal::ClientReaderFactory< ::txpool::OnAddReply>::Create(channel_.get(), rpcmethod_OnAdd_, context, request);
}

void Txpool::Stub::async::OnAdd(::grpc::ClientContext* context, const ::txpool::OnAddRequest* request, ::grpc::ClientReadReactor< ::txpool::OnAddReply>* reactor) {
  ::grpc::internal::ClientCallbackReaderFactory< ::txpool::OnAddReply>::Create(stub_->channel_.get(), stub_->rpcmethod_OnAdd_, context, request, reactor);
}

::grpc::ClientAsyncReader< ::txpool::OnAddReply>* Txpool::Stub::AsyncOnAddRaw(::grpc::ClientContext* context, const ::txpool::OnAddRequest& request, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::txpool::OnAddReply>::Create(channel_.get(), cq, rpcmethod_OnAdd_, context, request, true, tag);
}

::grpc::ClientAsyncReader< ::txpool::OnAddReply>* Txpool::Stub::PrepareAsyncOnAddRaw(::grpc::ClientContext* context, const ::txpool::OnAddRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderFactory< ::txpool::OnAddReply>::Create(channel_.get(), cq, rpcmethod_OnAdd_, context, request, false, nullptr);
}

::grpc::Status Txpool::Stub::Status(::grpc::ClientContext* context, const ::txpool::StatusRequest& request, ::txpool::StatusReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::StatusRequest, ::txpool::StatusReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Status_, context, request, response);
}

void Txpool::Stub::async::Status(::grpc::ClientContext* context, const ::txpool::StatusRequest* request, ::txpool::StatusReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::StatusRequest, ::txpool::StatusReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Status(::grpc::ClientContext* context, const ::txpool::StatusRequest* request, ::txpool::StatusReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Status_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::StatusReply>* Txpool::Stub::PrepareAsyncStatusRaw(::grpc::ClientContext* context, const ::txpool::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::StatusReply, ::txpool::StatusRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Status_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::StatusReply>* Txpool::Stub::AsyncStatusRaw(::grpc::ClientContext* context, const ::txpool::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncStatusRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status Txpool::Stub::Nonce(::grpc::ClientContext* context, const ::txpool::NonceRequest& request, ::txpool::NonceReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::txpool::NonceRequest, ::txpool::NonceReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_Nonce_, context, request, response);
}

void Txpool::Stub::async::Nonce(::grpc::ClientContext* context, const ::txpool::NonceRequest* request, ::txpool::NonceReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::txpool::NonceRequest, ::txpool::NonceReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Nonce_, context, request, response, std::move(f));
}

void Txpool::Stub::async::Nonce(::grpc::ClientContext* context, const ::txpool::NonceRequest* request, ::txpool::NonceReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_Nonce_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::txpool::NonceReply>* Txpool::Stub::PrepareAsyncNonceRaw(::grpc::ClientContext* context, const ::txpool::NonceRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::txpool::NonceReply, ::txpool::NonceRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_Nonce_, context, request);
}

::grpc::ClientAsyncResponseReader< ::txpool::NonceReply>* Txpool::Stub::AsyncNonceRaw(::grpc::ClientContext* context, const ::txpool::NonceRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncNonceRaw(context, request, cq);
  result->StartCall();
  return result;
}

Txpool::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::google::protobuf::Empty, ::types::VersionReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::google::protobuf::Empty* req,
             ::types::VersionReply* resp) {
               return service->Version(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::TxHashes, ::txpool::TxHashes, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::TxHashes* req,
             ::txpool::TxHashes* resp) {
               return service->FindUnknown(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::AddRequest, ::txpool::AddReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::AddRequest* req,
             ::txpool::AddReply* resp) {
               return service->Add(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::TransactionsRequest, ::txpool::TransactionsReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::TransactionsRequest* req,
             ::txpool::TransactionsReply* resp) {
               return service->Transactions(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::AllRequest, ::txpool::AllReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::AllRequest* req,
             ::txpool::AllReply* resp) {
               return service->All(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[5],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::google::protobuf::Empty, ::txpool::PendingReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::google::protobuf::Empty* req,
             ::txpool::PendingReply* resp) {
               return service->Pending(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[6],
      ::grpc::internal::RpcMethod::SERVER_STREAMING,
      new ::grpc::internal::ServerStreamingHandler< Txpool::Service, ::txpool::OnAddRequest, ::txpool::OnAddReply>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::OnAddRequest* req,
             ::grpc::ServerWriter<::txpool::OnAddReply>* writer) {
               return service->OnAdd(ctx, req, writer);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[7],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::StatusRequest, ::txpool::StatusReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::StatusRequest* req,
             ::txpool::StatusReply* resp) {
               return service->Status(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Txpool_method_names[8],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Txpool::Service, ::txpool::NonceRequest, ::txpool::NonceReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Txpool::Service* service,
             ::grpc::ServerContext* ctx,
             const ::txpool::NonceRequest* req,
             ::txpool::NonceReply* resp) {
               return service->Nonce(ctx, req, resp);
             }, this)));
}

Txpool::Service::~Service() {
}

::grpc::Status Txpool::Service::Version(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::types::VersionReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::FindUnknown(::grpc::ServerContext* context, const ::txpool::TxHashes* request, ::txpool::TxHashes* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::Add(::grpc::ServerContext* context, const ::txpool::AddRequest* request, ::txpool::AddReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::Transactions(::grpc::ServerContext* context, const ::txpool::TransactionsRequest* request, ::txpool::TransactionsReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::All(::grpc::ServerContext* context, const ::txpool::AllRequest* request, ::txpool::AllReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::Pending(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::txpool::PendingReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::OnAdd(::grpc::ServerContext* context, const ::txpool::OnAddRequest* request, ::grpc::ServerWriter< ::txpool::OnAddReply>* writer) {
  (void) context;
  (void) request;
  (void) writer;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::Status(::grpc::ServerContext* context, const ::txpool::StatusRequest* request, ::txpool::StatusReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status Txpool::Service::Nonce(::grpc::ServerContext* context, const ::txpool::NonceRequest* request, ::txpool::NonceReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace txpool

