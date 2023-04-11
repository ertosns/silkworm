// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: remote/kv.proto

#include "remote/kv.pb.h"
#include "remote/kv.grpc.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/sync_stream.h>
#include <gmock/gmock.h>
namespace remote {

class MockKVStub : public KV::StubInterface {
 public:
  MOCK_METHOD3(Version, ::grpc::Status(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::types::VersionReply* response));
  MOCK_METHOD3(AsyncVersionRaw, ::grpc::ClientAsyncResponseReaderInterface< ::types::VersionReply>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncVersionRaw, ::grpc::ClientAsyncResponseReaderInterface< ::types::VersionReply>*(::grpc::ClientContext* context, const ::google::protobuf::Empty& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD1(TxRaw, ::grpc::ClientReaderWriterInterface< ::remote::Cursor, ::remote::Pair>*(::grpc::ClientContext* context));
  MOCK_METHOD3(AsyncTxRaw, ::grpc::ClientAsyncReaderWriterInterface<::remote::Cursor, ::remote::Pair>*(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag));
  MOCK_METHOD2(PrepareAsyncTxRaw, ::grpc::ClientAsyncReaderWriterInterface<::remote::Cursor, ::remote::Pair>*(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq));
  MOCK_METHOD2(StateChangesRaw, ::grpc::ClientReaderInterface< ::remote::StateChangeBatch>*(::grpc::ClientContext* context, const ::remote::StateChangeRequest& request));
  MOCK_METHOD4(AsyncStateChangesRaw, ::grpc::ClientAsyncReaderInterface< ::remote::StateChangeBatch>*(::grpc::ClientContext* context, const ::remote::StateChangeRequest& request, ::grpc::CompletionQueue* cq, void* tag));
  MOCK_METHOD3(PrepareAsyncStateChangesRaw, ::grpc::ClientAsyncReaderInterface< ::remote::StateChangeBatch>*(::grpc::ClientContext* context, const ::remote::StateChangeRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(Snapshots, ::grpc::Status(::grpc::ClientContext* context, const ::remote::SnapshotsRequest& request, ::remote::SnapshotsReply* response));
  MOCK_METHOD3(AsyncSnapshotsRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::SnapshotsReply>*(::grpc::ClientContext* context, const ::remote::SnapshotsRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncSnapshotsRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::SnapshotsReply>*(::grpc::ClientContext* context, const ::remote::SnapshotsRequest& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(Range, ::grpc::Status(::grpc::ClientContext* context, const ::remote::RangeReq& request, ::remote::Pairs* response));
  MOCK_METHOD3(AsyncRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::RangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::RangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(DomainGet, ::grpc::Status(::grpc::ClientContext* context, const ::remote::DomainGetReq& request, ::remote::DomainGetReply* response));
  MOCK_METHOD3(AsyncDomainGetRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::DomainGetReply>*(::grpc::ClientContext* context, const ::remote::DomainGetReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncDomainGetRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::DomainGetReply>*(::grpc::ClientContext* context, const ::remote::DomainGetReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(HistoryGet, ::grpc::Status(::grpc::ClientContext* context, const ::remote::HistoryGetReq& request, ::remote::HistoryGetReply* response));
  MOCK_METHOD3(AsyncHistoryGetRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::HistoryGetReply>*(::grpc::ClientContext* context, const ::remote::HistoryGetReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncHistoryGetRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::HistoryGetReply>*(::grpc::ClientContext* context, const ::remote::HistoryGetReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(IndexRange, ::grpc::Status(::grpc::ClientContext* context, const ::remote::IndexRangeReq& request, ::remote::IndexRangeReply* response));
  MOCK_METHOD3(AsyncIndexRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::IndexRangeReply>*(::grpc::ClientContext* context, const ::remote::IndexRangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncIndexRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::IndexRangeReply>*(::grpc::ClientContext* context, const ::remote::IndexRangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(HistoryRange, ::grpc::Status(::grpc::ClientContext* context, const ::remote::HistoryRangeReq& request, ::remote::Pairs* response));
  MOCK_METHOD3(AsyncHistoryRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::HistoryRangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncHistoryRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::HistoryRangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(DomainRange, ::grpc::Status(::grpc::ClientContext* context, const ::remote::DomainRangeReq& request, ::remote::Pairs* response));
  MOCK_METHOD3(AsyncDomainRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::DomainRangeReq& request, ::grpc::CompletionQueue* cq));
  MOCK_METHOD3(PrepareAsyncDomainRangeRaw, ::grpc::ClientAsyncResponseReaderInterface< ::remote::Pairs>*(::grpc::ClientContext* context, const ::remote::DomainRangeReq& request, ::grpc::CompletionQueue* cq));
};

} // namespace remote

