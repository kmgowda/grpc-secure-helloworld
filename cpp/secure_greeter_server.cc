/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>
#include "helper.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#ifdef BAZEL_BUILD
#include "examples/protos/helloworld.grpc.pb.h"
#else
#include "helloworld.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

// Logic and data behind the server's behavior.
class SecureGreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

void RunServer(int argc, char** argv) {
  std::string server_address("0.0.0.0:50051");
  SecureGreeterServiceImpl service;
  SecureParams params;
  SecureCredentials certs;
  readCertParams(argc, argv, params);
  if (!parseSecureCredentials(params, certs)) {
      std::cout << "parseSecureCredentials failed " << std::endl;
      return;
  }
  grpc::SslServerCredentialsOptions ssl_opts;
  std::shared_ptr<grpc::ServerCredentials> creds;
  grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp ={certs.key, certs.cert};
  ssl_opts.pem_root_certs=certs.ca;
  ssl_opts.pem_key_cert_pairs.push_back(pkcp);
  creds = grpc::SslServerCredentials(ssl_opts);

  /*
   *  Invoking grpc::EnableDefaultHealthCheckService(true) , disable the reflections.
   *  grpc::EnableDefaultHealthCheckService(true);
  */

  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, creds);
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Secure Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer(argc, argv);

  return 0;
}
