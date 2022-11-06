#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "TinyRPC/Server/LocalRegistry.hpp"
#include "util/Config.hpp"
#include "TinyTCPServer2/Logger.hpp"

namespace TRPC
{
  TinyRPCServer::TinyRPCServer(
    const char* ip
  , unsigned short port 
  , unsigned int listenSize
  , unsigned int nNetIOReactors
  , TTCPS2::ThreadPool* const tp //工作线程池，单例
  , std::shared_ptr<LocalRegistry> localRegistry
  ) : httpServer(new TTCPS2::TinyHTTPServer(
        ip, port, listenSize, nNetIOReactors
      , std::dynamic_pointer_cast<TTCPS2::HTTPHandlerFactory, LocalRegistry>(localRegistry), tp))
    , localRegistry(localRegistry) {}

  int TinyRPCServer::run(){
    /// 注册服务

    auto conf = TTCPS2::loadConfigure("../conf/TinyRPCServer/rpcServer.properties");
    TTCPS2::HTTPRequest req;
    req.set(http_method::HTTP_POST)
       .set("/register")
       .set("RPC-Server-IP", conf["IP"])
       .set("RPC-Server-Port", conf["port"]);
    for(auto const& s : localRegistry->servicesName){
      req.append(s.data(),s.length())
         .append("\r\n",2);
    }

    conf = TTCPS2::loadConfigure("../conf/TinyRPCServer/registry.properties");
    ushort port;
    {std::stringstream ss;
    ss << conf["RegistryPort"];
    ss >> port;}
    ::TinyHTTPClient c(conf["RegistryIP"].c_str(), port);
    if(0>c.send(req)){
      TTCPS2_LOGGER.warn("TRPC::TinyRPCServer::run(): fail to send the HTTP request to register services.");
      return -1;
    }
    TTCPS2_LOGGER.trace("TRPC::TinyRPCServer::run(): the HTTP request to register services has been sent.");
    
    TTCPS2::HTTPResponse res;
    c.recv(res);
    if(res.status!=http_status::HTTP_STATUS_OK){
      TTCPS2_LOGGER.warn("TRPC::TinyRPCServer::run(): fail to register services.");
      return -1;
    }

    return httpServer->run();
  }

  int TinyRPCServer::shutdown(){
    // 注销服务

    auto conf = TTCPS2::loadConfigure("../conf/TinyRPCServer/rpcServer.properties");
    TTCPS2::HTTPRequest req;
    req.set(http_method::HTTP_POST)
       .set("/unregister")
       .set("RPC-Server-IP", conf["IP"])
       .set("RPC-Server-Port", conf["port"]);
    for(auto const& s : localRegistry->servicesName){
      req.append(s.data(),s.length())
         .append("\r\n",2);
    }

    conf = TTCPS2::loadConfigure("../conf/TinyRPCServer/registry.properties");
    ushort port;
    {std::stringstream ss;
    ss << conf["RegistryPort"];
    ss >> port;}
    ::TinyHTTPClient c(conf["RegistryIP"].c_str(), port);
    if(0>c.send(req)){
      TTCPS2_LOGGER.warn("TRPC::TinyRPCServer::shutdown(): fail to send the HTTP request to unregister services.");
      return -1;
    }
    TTCPS2_LOGGER.trace("TRPC::TinyRPCServer::shutdown(): the HTTP request to unregister services has been sent.");
    
    TTCPS2::HTTPResponse res;
    c.recv(res);
    if(res.status!=http_status::HTTP_STATUS_OK){
      TTCPS2_LOGGER.warn("TRPC::TinyRPCServer::shutdown(): fail to unregister services.");
      return -1;
    }

    return httpServer->shutdown();
  }
  
  TinyRPCServer::~TinyRPCServer(){
    delete httpServer;
  }
  
} // namespace TRPC
