#if !defined(_TinyRPCRegistry_hpp)
#define _TinyRPCRegistry_hpp

#include <unordered_map>
#include <vector>

namespace TTCPS2
{
  class TinyHTTPServer;
  class ThreadPool;
} // namespace TTCPS2

namespace TRPCS
{
  class TinyRPCRegistry
  {
  private:
    TTCPS2::TinyHTTPServer* httpServer;

    /// @brief <服务名称, 顺序表<服务端IP>>
    std::unordered_map<std::string, std::vector<std::string>> ip;

    /// @brief <服务名称, 顺序表<服务端端口>>
    std::unordered_map<std::string, std::vector<unsigned short>> port;
    
  public:

    TinyRPCRegistry(
      const char *ip
    , unsigned short port
    , unsigned int listenSize
    , unsigned int nNetIOReactors
    , TTCPS2::ThreadPool* tp);

    TinyRPCRegistry& Register(std::string const& serviceName, std::string const& ip, unsigned short port);
    TinyRPCRegistry& Drop(std::string const& serviceName, std::string const& ip, unsigned short port);
    
    virtual ~TinyRPCRegistry();
    
  };

} // namespace TRPCS

#endif // _TinyRPCRegistry_hpp
