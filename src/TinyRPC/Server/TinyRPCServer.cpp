#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "TinyRPC/Server/LocalRegistry.hpp"
#include "util/Config.hpp"

namespace TRPCS
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
    /// 注册任务

    TTCPS2::HTTPRequest req;
    req.set(http_method::HTTP_POST)
       .set("/register");
    for(auto const& s : localRegistry->servicesName){
      req.append(s.data(),s.length())
         .append("\r\n",2);
    }

    auto conf = TTCPS2::loadConfigure();
    ushort port;
    {std::stringstream ss;
    ss << conf["RegistryPort"];
    ss >> port;}
    ::TinyHTTPClient c(conf["RegistryIP"].c_str(), port);
    c.send(req);
    
    TTCPS2::HTTPResponse res;
    c.recv(res);

    return httpServer->run();
  }

  int TinyRPCServer::shutdown(){
    return httpServer->shutdown();
  }
  
  TinyRPCServer::~TinyRPCServer(){
    delete httpServer;
  }
  
} // namespace TRPCS
