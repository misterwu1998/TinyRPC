#if !defined(_TinyRPC_ConnectionPool_hpp)
#define _TinyRPC_ConnectionPool_hpp

#include <string>
#include <unordered_map>
#include <sstream>
#include "util/Singleton.hpp"
#include "util/Config.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"

class TinyHTTPClient;

QuickSingleton_prologue(ConnectionPool)

private:

  uint16_t registryPort;
  std::string registryIP;

  std::unordered_map<
    std::string,
    std::shared_ptr<TinyHTTPClient>
  > connections;

  ConnectionPool(std::string const& p){
    auto conf = loadConfigure(p);
    if(
      conf.count("IP")>0 &&
      conf.count("port")>0 &&
      !(conf["IP"].empty()) &&
      !(conf["port"].empty())
    ){
      registryIP = conf["IP"];
      
      std::stringstream ss;
      ss << conf["port"];
      ss >> registryPort;
    }
  }

public:

  static ConnectionPool& initOrGetPool(
    std::string const& registryConfPath = "../conf/registry.properties"
  ){
    static ConnectionPool p(registryConfPath);
    return p;
  }

  /// @brief 
  /// @param serviceName 服务接口名称
  /// @param dead 已被发现不能通信的连接
  /// @return 与RPC服务端的连接（不保证仍然可通信），或nullptr表示目前没有或获取失败
  std::shared_ptr<TinyHTTPClient> getConnection(
    std::string const& serviceName,
    std::shared_ptr<TinyHTTPClient> const& dead = nullptr
  ){
    if(connections.count(serviceName)>0){
      return connections[serviceName];
    }//本地还没有，得向注册中心索要
    
    HTTPRequest req;
    req.set(http_method::HTTP_GET)
       .set("/find")
       .set("Service-Name",serviceName);
    if(dead){
      std::string deadPort;{
        std::stringstream ss;
        ss << dead->getServerPort();
        ss >> deadPort;
      }
      req.set("Dead-IP", dead->getServerIP())
         .set("Dead-port", deadPort);
    }

    HTTPResponse res;
    TinyHTTPClient toRegistry(
      registryIP.c_str(),
      registryPort
    );
    if(
      0>toRegistry.send(req) ||
      0>toRegistry.recv(res) ||
      res.status != http_status::HTTP_STATUS_OK ||
      res.header.count("IP")<1 ||
      res.header.count("port")<1
    ){
      return nullptr;
    }

    uint16_t port;{
      std::stringstream ss;
      ss << res.header.find("port")->second;
      ss >> port;
    }
    connections[serviceName] = std::make_shared<TinyHTTPClient>(
      res.header.find("IP")->second.c_str(),
      port
    );
    return connections[serviceName];
  }

QuickSingleton_epilogue

#endif // _TinyRPC_ConnectionPool_hpp
