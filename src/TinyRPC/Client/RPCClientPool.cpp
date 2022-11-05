#include "TinyRPC/Client/RPCClientPool.hpp"
#include "TinyRPC/Client/RPCClientMacro.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"

namespace TRPC
{
    int RPCClientPool::add(std::string const& serviceName, std::shared_ptr<::TinyHTTPClient> connection){
      std::lock_guard<std::mutex> lg(m_client);

      auto it = clients.find(serviceName);
      if(it==clients.end()){
        clients.insert({serviceName,connection});
        return 1;
      }else{
        it->second = connection;
        return 0;
      }
    }

    std::shared_ptr<::TinyHTTPClient> RPCClientPool::find(std::string const& serviceName){
      std::lock_guard<std::mutex> lg(m_client);
      
      auto it = clients.find(serviceName);
      if(it==clients.end()){
        return nullptr;
      }else{
        return it->second;
      }
    }

    int RPCClientPool::remove(std::string const& serviceName){
      std::lock_guard<std::mutex> lg(m_client);
      
      auto it = clients.find(serviceName);
      if(it==clients.end()){
        return 0;
      }else{
        clients.erase(it);
        return 1;
      }
    }

    int RPCClientPool::add(std::string const& serviceName, std::string const& ip, std::string const& port){
      std::lock_guard<std::mutex> lg(m_addr);

      auto i = IPs.find(serviceName);
      auto p = ports.find(serviceName);
      if(i==IPs.end() || p==ports.end()){
        IPs.insert({serviceName,ip});
        ports.insert({serviceName,port});
        return 1;
      }else{
        i->second = ip;
        p->second = ip;
        return 0;
      }
    }

    int RPCClientPool::find(std::string const& serviceName, std::string& ip, std::string& port){
      std::lock_guard<std::mutex> lg(m_addr);
      
      auto i = IPs.find(serviceName);
      auto p = ports.find(serviceName);
      if(i==IPs.end() || p==ports.end()){
        return 0;
      }else{
        ip = i->second;
        port = p->second;
        return 1;
      }
    }

    int RPCClientPool::remove(std::string const& serviceName, std::string const& ip, std::string const& port){
      std::lock_guard<std::mutex> lg(m_addr);
      
      auto i = IPs.find(serviceName);
      auto p = ports.find(serviceName);
      if(i==IPs.end() || p==ports.end()){
        return 0;
      }else{
        IPs.erase(i);
        ports.erase(p);
        return 1;
      }
    }
  
} // namespace TRPC

