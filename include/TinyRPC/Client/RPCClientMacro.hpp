#if !defined(_RPCClientMacro_hpp)
#define _RPCClientMacro_hpp

#if 1 // for coding

#include "TinyRPC/ServiceMacro.hpp"
QUICK_RPC_0_SERVICE_NAME(ServiceName)
QUICK_RPC_1_REQUEST_BEFORE_FIELDS
int i;
QUICK_RPC_2_REQUEST_FIELDS_SERIALIZED(i)
QUICK_RPC_3_RESPONSE_BEFORE_FIELDS
int i;
QUICK_RPC_4_RESPONSE_FIELDS_SERIALIZED(i)

#endif

#include "TinyRPC/Client/RPCClientPool.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "util/Config.hpp"
#include <sstream>
#include "TinyRPC/Cerealization.hpp"
#include "util/Buffer.hpp"
#include "TinyTCPServer2/Logger.hpp"

#define QUICK_RPC_CLIENT(ServiceName)\
std::shared_ptr< ServiceName ::Response> ServiceName ::Do(std::shared_ptr< ServiceName ::Request> req){\
  if(!req) return nullptr;\
\
  auto& pool = TRPC::RPCClientPool::getPool();\
  auto conn = pool.get( #ServiceName );\
  if(!conn){/*目前没有可用的连接 */\
    \
    /* 向注册中心询问服务端地址 */\
    auto reqToRegistry = std::make_shared<TTCPS2::HTTPRequest>();\
    reqToRegistry->set(http_method::HTTP_GET)\
        .set("/find")\
        .set("RPC-Service-Name", #ServiceName );\
    auto registryConf = TTCPS2::loadConfigure("../conf/TinyRPCClient/registry.properties");\
    ushort pp; { std::stringstream ss; ss << registryConf["RegistryPort"]; ss >> pp; }\
    ::TinyHTTPClient c(registryConf["RegistryIP"].c_str(), pp);\
    if(0>c.send(*reqToRegistry)){\
      TTCPS2_LOGGER.error(std::string( #ServiceName ) + "::Do(): fail to send to Registry at {0}:{1}.", registryConf["RegistryIP"],registryConf["RegistryPort"]);\
      exit(-1);\
    }\
\
    /* 从注册中心收取服务端地址 */\
    auto resFromRegistry = std::make_shared<TTCPS2::HTTPResponse>();\
    if(0>c.recv(*resFromRegistry)){\
      TTCPS2_LOGGER.error(std::string( #ServiceName ) + "::Do(): fail to recv from Registry at {0}:{1}.", registryConf["RegistryIP"],registryConf["RegistryPort"]);\
      exit(-1);\
    }\
    if(resFromRegistry->status!=http_status::HTTP_STATUS_OK\
    || resFromRegistry->header.count("RPC-Server-IP")<1\
    || resFromRegistry->header.count("RPC-Server-Port")<1){\
      return nullptr;\
    }\
\
    /* 向RPC服务端发起连接 */\
    std::string ip,port;\
    ip = resFromRegistry->header.find("RPC-Server-IP")->second;\
    port = resFromRegistry->header.find("RPC-Server-Port")->second;\
    { std::stringstream ss; ss << port; ss >> pp; }\
    conn = std::make_shared<::TinyHTTPClient>(ip.c_str(), pp);\
  }\
\
  std::string seri; {\
  std::stringstream ss; TRPC::serialize(*req, ss);\
  seri = ss.str(); }/*delete ss */\
\
  auto httpReq = std::make_shared<TTCPS2::HTTPRequest>();\
  auto httpRes = std::make_shared<TTCPS2::HTTPResponse>();\
  httpReq->set(http_method::HTTP_POST)\
          .set(std::string("/") + #ServiceName)\
          .append(seri.data(), seri.size());\
  if(0 > conn->send(*httpReq)\
  || 0 > conn->recv(*httpRes)){/*这个服务端不可用 */\
    TTCPS2_LOGGER.warn(std::string( #ServiceName ) + "::Do(): 0 > conn->send(*httpReq) || 0 > conn->recv(*httpRes)");\
    /* 得告知注册中心 */\
\
    /* 向注册中心询问服务端地址 */\
    auto reqToRegistry = std::make_shared<TTCPS2::HTTPRequest>();\
    reqToRegistry->set(http_method::HTTP_GET)\
        .set("/find")\
        .set("RPC-Service-Name", #ServiceName )\
        .set("RPC-Server-IP", conn->getServerIP()) /* 捎上这个失效的服务端地址 */\
        .set("RPC-Server-Port", std::to_string(conn->getServerPort())); /* 捎上这个失效的服务端地址 */\
    auto registryConf = TTCPS2::loadConfigure("../conf/TinyRPCClient/registry.properties");\
    ushort pp; { std::stringstream ss; ss << registryConf["RegistryPort"]; ss >> pp; }\
    ::TinyHTTPClient c(registryConf["RegistryIP"].c_str(), pp);\
    if(0>c.send(*reqToRegistry)){\
      TTCPS2_LOGGER.error(std::string( #ServiceName ) + "::Do(): fail to send to Registry at {0}:{1}.", registryConf["RegistryIP"],registryConf["RegistryPort"]);\
      exit(-1);\
    }\
\
    /* 从注册中心收取服务端地址 */\
    auto resFromRegistry = std::make_shared<TTCPS2::HTTPResponse>();\
    if(0>c.recv(*resFromRegistry)){\
      TTCPS2_LOGGER.error(std::string( #ServiceName ) + "::Do(): fail to recv from Registry at {0}:{1}.", registryConf["RegistryIP"],registryConf["RegistryPort"]);\
      exit(-1);\
    }\
    if(resFromRegistry->status!=http_status::HTTP_STATUS_OK\
    || resFromRegistry->header.count("RPC-Server-IP")<1\
    || resFromRegistry->header.count("RPC-Server-Port")<1){\
      return nullptr;\
    }\
\
    /* 暂不建立新连接到RPC服务端 */\
    return nullptr;\
  }\
  else{/*这个服务端还是可用的 */\
    TTCPS2_LOGGER.trace(std::string( #ServiceName ) + "::Do(): RPC server available.");\
    pool.put( #ServiceName , conn);/*下面已经不需要这条连接了 */\
  }\
\
  if(httpRes->body==nullptr || httpRes->body->getLength()<1){\
    return nullptr;\
  }\
  auto res = std::make_shared< ServiceName ::Response>();\
  { std::stringstream ss; ss << *(httpRes->body); TRPC::deserialize(ss, *res); }\
  return res;\
}

#endif // _RPCClientMacro_hpp

#if 1
QUICK_RPC_CLIENT(ServiceName)
#endif
