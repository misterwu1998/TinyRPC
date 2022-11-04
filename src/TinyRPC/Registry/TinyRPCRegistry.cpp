#include "TinyRPC/Registry/TinyRPCRegistry.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"

namespace TRPCS
{
  TinyRPCRegistry::TinyRPCRegistry(
    const char *ip
  , unsigned short port
  , unsigned int listenSize
  , unsigned int nNetIOReactors
  , TTCPS2::ThreadPool* tp){
    auto httpSettings = std::make_shared<TTCPS2::HTTPHandlerFactory>();

    // 服务端注册服务
    httpSettings->route(http_method::HTTP_POST, "/register", [this](std::shared_ptr<TTCPS2::HTTPRequest> req){
      // 艹，回调的参数改成HTTPRequest后，get不到客户端的socket了，想个办法解决这个问题
    });

    // 客户端查询服务端的地址
    httpSettings->route(http_method::HTTP_POST, "/find", [this](std::shared_ptr<TTCPS2::HTTPRequest> req){});

    httpServer = new TTCPS2::TinyHTTPServer(ip,port,listenSize,nNetIOReactors,httpSettings,tp);
  }

  TinyRPCRegistry& TinyRPCRegistry::Register(std::string const& serviceName, std::string const& ip, unsigned short port){
    for(uint32_t i=0; i < this->ip[serviceName].size(); ++i){
      if(this->ip[serviceName][i] == ip){
        if(this->port[serviceName][i] == port){//已有，不必再注册
          return *this;
        }
      }
    }
    this->ip[serviceName].push_back(ip);
    this->port[serviceName].push_back(port);
    return *this;
  }

  TinyRPCRegistry& TinyRPCRegistry::Drop(std::string const& serviceName, std::string const& ip, unsigned short port){
    if(this->ip.count(serviceName)<1){
      return *this;
    }
    auto a = this->ip[serviceName].begin();
    auto p = this->port[serviceName].begin();
    for(uint32_t i=0; i < this->ip[serviceName].size(); ++i){
      if(*a == ip && port == *p){//找到了
        this->ip[serviceName].erase(a);
        this->port[serviceName].erase(p);
        break;
      }
      a++; p++;
    }
    return *this;
  }

  TinyRPCRegistry::~TinyRPCRegistry(){
    delete httpServer;
  }

} // namespace TRPCS
