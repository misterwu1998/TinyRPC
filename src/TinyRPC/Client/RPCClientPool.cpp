#include "TinyRPC/Client/RPCClientPool.hpp"
#include "TinyRPC/Client/RPCClientMacro.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"

namespace TRPC
{
    int RPCClientPool::put(std::string const& serviceName, std::shared_ptr<TinyHTTPClient>& connection){
      std::lock_guard<std::mutex> lg(m_connections);

      auto range = connections.equal_range(serviceName);
      for(auto it = range.first; it!=range.second; it++){
        if(it->second==connection){
          return 0;
        }
      }
      connections.insert({serviceName,connection});
      connection = nullptr;
      return 1;
    }
    
    std::shared_ptr<TinyHTTPClient> RPCClientPool::get(std::string const& serviceName){
      std::lock_guard<std::mutex> lg(m_connections);

      auto it = connections.find(serviceName);
      if(it==connections.end()){
        return nullptr;
      }else{
        return it->second;
      }
    }

} // namespace TRPC

