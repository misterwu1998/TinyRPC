#if !defined(_RPCClientPool_hpp)
#define _RPCClientPool_hpp

#include "util/Singleton.hpp"
#include <mutex>
#include <unordered_map>
#include <memory>

class TinyHTTPClient;

namespace TRPC
{
  QuickSingleton_prologue(RPCClientPool)
  
  private:
    /// @brief <服务名称,HTTP客户端>
    std::unordered_multimap<std::string, std::shared_ptr<TinyHTTPClient>> connections;
    std::mutex m_connections;

  public:

    /// @brief 放入一个
    /// @param connection 放入后，被置空
    /// @return 1：成功；0：已在池内；-1：出错
    int put(std::string const& serviceName, std::shared_ptr<TinyHTTPClient>& connection);
    
    /// @brief 取走一个
    /// @param serviceName 
    /// @return 如果暂时没有，就返回nullptr
    std::shared_ptr<TinyHTTPClient> get(std::string const& serviceName);

    static RPCClientPool& getPool(){
      static RPCClientPool instance;
      return instance;
    }

  QuickSingleton_epilogue

} // namespace TRPC

#endif // _RPCClientPool_hpp
