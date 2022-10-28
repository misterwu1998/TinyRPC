#if !defined(_TinyRPCServer_hpp)
#define _TinyRPCServer_hpp

#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>

class TTCPS2::TinyHTTPServer;
class TTCPS2::ThreadPool;

namespace TRPCS
{
  class TinyRPCServer
  {
  private:
    TTCPS2::TinyHTTPServer* httpServer;
  
  public:
  
    /// @brief 
    /// @param ip 
    /// @param port 
    /// @param listenSize 
    /// @param nNetIOReactors 
    /// @param tp 线程池，单例
    /// @param services <请求的方法名, 方法实现<返回的状态码 (读取请求数据、放回响应数据的缓冲区)>>
    TinyRPCServer(
        const char* ip
      , unsigned short port 
      , unsigned int listenSize
      , unsigned int nNetIOReactors
      , TTCPS2::ThreadPool* const tp //线程池是单例
      , std::unordered_map<std::string, std::function<int (std::iostream&)>> const& services
    );
    virtual ~TinyRPCServer();
  };
    
} // namespace TRPCS

#endif // _TinyRPCServer_hpp
