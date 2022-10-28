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
      , TTCPS2::ThreadPool* const tp //线程池是单例
      , std::unordered_map<std::string, std::function<int (std::iostream&)>> const& services
    ){
      
    }
  
  TinyRPCServer::~TinyRPCServer(){}
  
} // namespace TRPCS
