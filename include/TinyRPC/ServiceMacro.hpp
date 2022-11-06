#if !defined(_ServiceMacro_hpp)
#define _ServiceMacro_hpp

/**
 * 宏，用于快捷定义或实现服务接口和消息对象
*/

#include <memory>

#define QUICK_RPC_0_SERVICE_NAME(ServiceName)\
class ServiceName{\
public:\
\
  class Request;\
  class Response;\
  /** @brief 客户端实现：服务发现、向服务端请求服务；服务端实现：当前服务的实现 @param r @return 失败时，返回nullptr */\
  static std::shared_ptr<Response> Do(std::shared_ptr<Request> r);

#define QUICK_RPC_1_REQUEST_BEFORE_FIELDS\
  class Request{\
  public:

#define QUICK_RPC_2_REQUEST_FIELDS_SERIALIZED(...)\
  public:\
    template<typename Archive>\
    void serialize(Archive & a){\
      a(/*fields to serialize*/__VA_ARGS__);\
    }\
  };
  
#define QUICK_RPC_3_RESPONSE_BEFORE_FIELDS\
  class Response{\
  public:\
    
#define QUICK_RPC_4_RESPONSE_FIELDS_SERIALIZED(...)\
  public:\
    template<typename Archive>\
    void serialize(Archive & a){\
      a(/*fields to serialize*/__VA_ARGS__);\
    }\
  };\
};

#define QUICK_RPC_RETURN_TYPE(ServiceName)\
std::shared_ptr<ServiceName :: Response>

#define QUICK_RPC_ARG_TYPE(ServiceName)\
std::shared_ptr<ServiceName :: Request>

#endif // _ServiceMacro_hpp

#if 0 // example

#include <string>

QUICK_RPC_0_SERVICE_NAME(abc)
QUICK_RPC_1_REQUEST_BEFORE_FIELDS
int id;
std::string name;
QUICK_RPC_2_REQUEST_FIELDS_SERIALIZED(id,name)
QUICK_RPC_3_RESPONSE_BEFORE_FIELDS
// int _status_code;//optional
double score;
QUICK_RPC_4_RESPONSE_FIELDS_SERIALIZED(score)

#endif //example
