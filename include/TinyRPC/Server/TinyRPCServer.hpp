#if !defined(_TinyRPCServer_hpp)
#define _TinyRPCServer_hpp

//遇到internal error后，睡多少秒后才exit（留点时间把500发回给客户端）
#define SLEEP_AFTER_INTERNAL_ERROR (2)

#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>

class TTCPS2::TinyHTTPServer;
class TTCPS2::ThreadPool;

namespace TRPCS
{
  class LocalRegistry;

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
    /// @param tp 工作线程池，单例
    /// @param localRegistry 
    TinyRPCServer(
        const char* ip
      , unsigned short port 
      , unsigned int listenSize
      , unsigned int nNetIOReactors
      , TTCPS2::ThreadPool* const tp //工作线程池，单例
      , std::shared_ptr<LocalRegistry> localRegistry
    );
    int run();
    int shutdown();
    virtual ~TinyRPCServer();
  };
    
} // namespace TRPCS

#endif // _TinyRPCServer_hpp
