#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"

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
      , std::dynamic_pointer_cast<TTCPS2::HTTPHandlerFactory, LocalRegistry>(localRegistry), tp
  )){}

  int TinyRPCServer::run(){
    /// @todo 注册任务

    return httpServer->run();
  }

  int TinyRPCServer::shutdown(){
    return httpServer->shutdown();
  }
  
  TinyRPCServer::~TinyRPCServer(){
    delete httpServer;
  }
  
} // namespace TRPCS
