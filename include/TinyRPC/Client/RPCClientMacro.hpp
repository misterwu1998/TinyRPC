#if !defined(_RPCClientMacro_hpp)
#define _RPCClientMacro_hpp

#if 0 // for coding

#include "TinyRPC/ServiceMacro.hpp"
QuickRPC_0_service_name(ABC)
QuickRPC_1_Request_before_fields
int i;
QuickRPC_2_Request_fields_serialized(i)
QuickRPC_3_Response_before_fields
int i;
QuickRPC_4_Response_fields_serialized(i)

#endif

#include "TinyRPC/Client/RPCClientPool.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "util/Config.hpp"
#include <sstream>
#include "TinyRPC/Cerealization.hpp"
#include "util/Buffer.hpp"

#define QuickRPC_Client(ServiceName)\
std::shared_ptr<ServiceName::Response> ServiceName::Do(std::shared_ptr<ServiceName::Request> req){\
  if(!req) return nullptr;\
\
  std::string ip,port;\
  auto& pool = TRPC::RPCClientPool::get();\
  auto client = pool.find( #ServiceName );\
  if(!client){/*还未有可用的连接 */\
\
    if(1>pool.find( #ServiceName , ip,port)){/*还没有这个服务的地址 */\
      auto req = std::make_shared<TTCPS2::HTTPRequest>();\
      req->set(http_method::HTTP_GET)\
          .set("/find");\
      \
      auto registryConf = TTCPS2::loadConfigure("../conf/Client/registry.properties");\
      ushort pp; std::stringstream ss; ss << registryConf["RegistryPort"]; ss >> pp;\
      ::TinyHTTPClient c(registryConf["RegistryIP"].c_str(), pp);\
      c.send(*req);\
\
      auto res = std::make_shared<TTCPS2::HTTPResponse>();\
      c.recv(*res);\
      if(res->status!=http_status::HTTP_STATUS_OK\
      || res->header.count("RPC-Server-IP")<1\
      || res->header.count("RPC-Server-Port")<1){\
        return nullptr;\
      }\
\
      ip = res->header.find("RPC-Server-IP")->second;\
      port = res->header.find("RPC-Server-Port")->second;\
      /* 后面证实这个地址可用了，才追加 */\
    }\
\
    /* 建立连接 */\
    ushort pp; std::stringstream ss; ss << port; ss >> pp;\
    client = std::make_shared<::TinyHTTPClient>(ip.c_str(), pp);\
    /* 后面证实这个连接可用了，才追加 */\
  }\
\
  std::shared_ptr<TTCPS2::HTTPRequest> httpReq;\
  {std::string sss;\
  {std::stringstream ss; TRPC::serialize(*req, ss);\
  sss = ss.str();}\
  httpReq = std::make_shared<TTCPS2::HTTPRequest>();\
  httpReq->set(http_method::HTTP_POST)\
          .set( #ServiceName )\
          .append(sss.data(), sss.length());}\
  if(0 > client->send(*httpReq)){/*不可用 */\
    return nullptr;\
  }\
\
  auto httpRes = std::make_shared<TTCPS2::HTTPResponse>();\
  if(0 > client->recv(*httpRes)){/*不可用 */\
    return nullptr;\
  }\
  pool.add( #ServiceName , client);\
  if(! (ip.empty()||port.empty())) pool.add( #ServiceName ,ip,port);\
\
  if(httpRes->body==nullptr || httpRes->body->getLength()<1){\
    return nullptr;\
  }\
  auto res = std::make_shared<ServiceName::Response>();\
  {std::stringstream ss; ss << *(httpRes->body);\
  TRPC::deserialize(ss, *res);}\
  return res;\
}

#endif // _RPCClientMacro_hpp

#if 0
QuickRPC_Client(ABC)
#endif
