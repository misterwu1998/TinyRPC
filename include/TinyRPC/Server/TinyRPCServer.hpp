#if !defined(_TinyRPCServer_hpp)
#define _TinyRPCServer_hpp

#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_set>
#include <memory>

namespace TTCPS2{
  class TinyHTTPServer;
  class ThreadPool;
}

namespace TRPC
{
  class LocalRegistry;

  class TinyRPCServer
  {
  private:
    TTCPS2::TinyHTTPServer* httpServer;
  
  protected:
    std::shared_ptr<LocalRegistry> localRegistry;
  
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
    
} // namespace TRPC

#endif // _TinyRPCServer_hpp
