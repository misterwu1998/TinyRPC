#if !defined(_RPCClientPool_hpp)
#define _RPCClientPool_hpp

#include "util/Singleton.hpp"
#include <mutex>
#include <unordered_map>

class TinyHTTPClient;

namespace TRPC
{
  QuickSingleton_prologue(RPCClientPool)
  
  private:

    std::mutex m_client;

    /// @brief <服务名称,HTTP客户端>
    std::unordered_map<std::string, std::shared_ptr<::TinyHTTPClient>> clients;

    std::mutex m_addr;

    /// @brief <服务名称,服务端IP>
    std::unordered_map<std::string, std::string> IPs;

    /// @brief <服务名称,服务端端口>
    std::unordered_map<std::string, std::string> ports;

  public:

    /// @return 1：成功添加；0：成功替换原来的；-1：出错
    int add(std::string const& serviceName, std::shared_ptr<::TinyHTTPClient> connection);

    /// @brief 
    /// @param serviceName 
    /// @return 还没有TinyHTTPClient的话，nullptr
    std::shared_ptr<::TinyHTTPClient> find(std::string const& serviceName);

    /// @brief 
    /// @param serviceName 
    /// @return 1：成功移除；0：不需要移除；-1：出错
    int remove(std::string const& serviceName);

    /// @return 1：成功添加；0：成功替换原来的地址；-1：出错
    int add(std::string const& serviceName, std::string const& ip, std::string const& port);

    /// @return 1表示找到了；0表示还没有这个服务的地址
    int find(std::string const& serviceName, std::string& ip, std::string& port);

    /// @brief 如果服务serviceName的地址是{ip,port}, 就删掉它
    /// @param serviceName 
    /// @param ip 
    /// @param port 
    /// @return 1：成功添加；0：不需要移除；-1：出错
    int remove(std::string const& serviceName, std::string const& ip, std::string const& port);
  
    static RPCClientPool& get(){
      static RPCClientPool instance;
      return instance;
    }

  QuickSingleton_epilogue

} // namespace TRPC

#endif // _RPCClientPool_hpp
