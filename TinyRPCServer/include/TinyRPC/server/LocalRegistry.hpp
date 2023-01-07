#if !defined(_TinyRPC_LocalRegistry_hpp)
#define _TinyRPC_LocalRegistry_hpp

#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include "util/Singleton.hpp"

class HTTPRequest;
class HTTPResponse;

QuickSingleton_prologue(LocalRegistry)

public:
  static auto& getCallbackMap(){
    static std::unordered_map<
      std::string,
      std::shared_ptr<HTTPResponse> (*)(std::shared_ptr<HTTPRequest>)
    > m;
    return m;
  }

QuickSingleton_epilogue

class LocalRegistryAction
{
public:
  LocalRegistryAction(
    std::string const& serviceName,
    std::shared_ptr<HTTPResponse> (*callback)(
      std::shared_ptr<HTTPRequest>
    )
  ){
    LocalRegistry::getCallbackMap()[serviceName] = callback;
  }
};

#endif // _TinyRPC_LocalRegistry_hpp
