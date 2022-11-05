#include "TinyRPC/Registry/TinyRPCRegistry.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include <sstream>
#include "util/Buffer.hpp"

namespace TRPC
{
  TinyRPCRegistry::TinyRPCRegistry(
    const char *ip
  , unsigned short port
  , unsigned int listenSize
  , unsigned int nNetIOReactors
  , TTCPS2::ThreadPool* tp)
  : roundRobin(0){
    auto httpSettings = std::make_shared<TTCPS2::HTTPHandlerFactory>();

    // 服务端注册服务
    httpSettings->route(http_method::HTTP_POST, "/register", [this](std::shared_ptr<TTCPS2::HTTPRequest> req){
      // req的结构：
      // POST /register HTTP/1.1
      // RPC-Server-IP: 【IP地址】
      // RPC-Server-Port: 【端口号】
      // Content-Length: 【body长度】
      // 【其它header……】
      // 
      // 【服务1的名称】
      // 【服务2的名称】
      // ……

      auto res = std::make_shared<TTCPS2::HTTPResponse>();
      if(!req) return res;
      if(req->header.count("RPC-Server-IP")<1 || req->header.count("RPC-Server-Port")<1 || req->body==nullptr){
        res->set(http_status::HTTP_STATUS_BAD_REQUEST);
        return res;
      }
      auto const& ip = req->header.find("RPC-Server-IP")->second;
      auto const& port = req->header.find("RPC-Server-Port")->second;
      std::stringstream body; body << *(req->body);
      while(! body.eof()){
        std::string line; std::getline(body,line);
        int32_t cut = line.length();
        while(cut>0 && (line[cut-1]=='\r' || line[cut-1]=='\n')) cut--;//cut: 截后长度
        if(1>cut) break;
        auto serviceName = line.substr(0,cut);

        this->Register(serviceName,ip,port);
      }
      
      res->set(http_status::HTTP_STATUS_OK);
      return res;
    });

    // 客户端查询服务端的地址
    httpSettings->route(http_method::HTTP_POST, "/find", [this](std::shared_ptr<TTCPS2::HTTPRequest> req){
      // req的结构：
      // GET /find HTTP/1.1
      // RPC-Service-Name: 【服务的名称】
      // RPC-Server-IP: 【客户端声称连不上的IP地址，如果客户端刚刚连接RPC服务端失败的话，就有这个header】
      // RPC-Server-Port: 【客户端声称连不上的端口号，如果客户端刚刚连接RPC服务端失败的话，就有这个header】
      // Content-Length: 0
      // 【其它header……】
      // 
      
      auto res = std::make_shared<TTCPS2::HTTPResponse>();
      if(!req) return res;
      if(req->header.count("RPC-Service-Name")<1){
        return res;
      }
      auto const& serviceName = req->header.find("RPC-Service-Name")->second;
      res->set(http_status::HTTP_STATUS_OK);

      std::string ip,port;
      if(1 > this->Find(serviceName,ip,port)){
        return res;
      }
      res->set("RPC-Server-IP",ip)
          .set("RPC-Server-Port",port);
      return res;
      
      // res的结构：
      // HTTP/1.1 200 OK
      // RPC-Server-IP: 【IP地址，如果找得到；如果没有可用的RPC服务端，就没有这个header】
      // RPC-Server-Port: 【端口号，如果找得到；如果没有可用的RPC服务端，就没有这个header】
      // Content-Length: 0
      // 【其它header……】
      // 
    });

    httpServer = new TTCPS2::TinyHTTPServer(ip,port,listenSize,nNetIOReactors,httpSettings,tp);
  }

  TinyRPCRegistry& TinyRPCRegistry::Register(std::string const& serviceName, std::string const& ip, std::string const& port){
    std::lock_guard<std::mutex> lg(m_addr);

    for(uint32_t i=0; i < this->IPs[serviceName].size(); ++i){
      if(this->IPs[serviceName][i] == ip){
        if(this->ports[serviceName][i] == port){//已有，不必再注册
          return *this;
        }
      }
    }
    this->IPs[serviceName].push_back(ip);
    this->ports[serviceName].push_back(port);
    return *this;
  }

  int32_t TinyRPCRegistry::Find(std::string const& serviceName, std::string& ip, std::string& port){
    std::lock_guard<std::mutex> lg(m_addr);
    
    if(IPs.count(serviceName)<1 || ports.count(serviceName)<1) return 0;
    if(IPs[serviceName].empty() || ports[serviceName].empty()) return 0;

    auto index = roundRobin % IPs.size();
    ip = IPs[serviceName][index];
    port = ports[serviceName][index];

    roundRobin++;
    return 1;
  }

  TinyRPCRegistry& TinyRPCRegistry::Drop(std::string const& serviceName, std::string const& ip, std::string const& port){
    std::lock_guard<std::mutex> lg(m_addr);
    
    if(this->IPs.count(serviceName)<1){
      return *this;
    }
    auto a = this->IPs[serviceName].begin();
    auto p = this->ports[serviceName].begin();
    for(uint32_t i=0; i < this->IPs[serviceName].size(); ++i){
      if(*a == ip && port == *p){//找到了
        this->IPs[serviceName].erase(a);
        this->ports[serviceName].erase(p);
        break;
      }
      a++; p++;
    }
    return *this;
  }

  TinyRPCRegistry::~TinyRPCRegistry(){
    delete httpServer;
  }

} // namespace TRPC
