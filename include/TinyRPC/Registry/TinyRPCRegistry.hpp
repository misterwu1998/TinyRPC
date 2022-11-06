#if !defined(_TinyRPCRegistry_hpp)
#define _TinyRPCRegistry_hpp

#include <unordered_map>
#include <vector>
#include <mutex>

namespace TTCPS2
{
  class TinyHTTPServer;
  class ThreadPool;
} // namespace TTCPS2

namespace TRPC
{
  class TinyRPCRegistry
  {
  private:
    TTCPS2::TinyHTTPServer* httpServer;

    /// @brief 保护: IPs ports roundRobin
    std::mutex m_addr;

    /// @brief <服务名称, 顺序表<服务端IP>>
    std::unordered_map<std::string, std::vector<std::string>> IPs;

    /// @brief <服务名称, 顺序表<服务端端口>>
    std::unordered_map<std::string, std::vector<std::string>> ports;

    uint32_t roundRobin;
    
  public:

    TinyRPCRegistry(
      const char *ip
    , unsigned short port
    , unsigned int listenSize
    , unsigned int nNetIOReactors
    , TTCPS2::ThreadPool* tp);

    TinyRPCRegistry& Register(std::string const& serviceName, std::string const& ip, std::string const& port);

    /// @brief 以轮询的方式返回一个RPC服务端的地址
    /// @param serviceName 
    /// @param ip 
    /// @param port 
    /// @return 0：没找到；1：找到了
    int32_t Find(std::string const& serviceName, std::string& ip, std::string& port);

    TinyRPCRegistry& Drop(std::string const& serviceName, std::string const& ip, std::string const& port);

    int run();
    int shutdown();
    
    virtual ~TinyRPCRegistry();
    
  };

} // namespace TRPC

#endif // _TinyRPCRegistry_hpp
