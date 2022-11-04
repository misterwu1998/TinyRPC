#if !defined(_ServiceMacro_hpp)
#define _ServiceMacro_hpp

/**
 * 宏，用于快捷定义或实现服务接口和消息对象
*/

#include <memory>

#define QuickRPC_0_service_name(ServiceName)\
class ServiceName{\
public:\
\
  class Request;\
  class Response;\
  /** @brief 客户端实现：服务发现、向服务端请求服务；服务端实现：当前服务的实现 @param r @return 失败时，返回nullptr */\
  static std::shared_ptr<Response> Do(std::shared_ptr<Request> r);

#define QuickRPC_1_Request_before_fields\
  class Request{\
  public:

#define QuickRPC_2_Request_fields_serialized(...)\
  public:\
    template<typename Archive>\
    void serialize(Archive & a){\
      a(/*fields to serialize*/__VA_ARGS__);\
    }\
  };
  
#define QuickRPC_3_Response_before_fields\
  class Response{\
  public:\
    
#define QuickRPC_4_Response_fields_serialized(...)\
  public:\
    template<typename Archive>\
    void serialize(Archive & a){\
      a(/*fields to serialize*/__VA_ARGS__);\
    }\
  };\
};

#define QuickRPC_ReturnType(ServiceName)\
std::shared_ptr<ServiceName :: Response>

#define QuickRPC_ArgType(ServiceName)\
std::shared_ptr<ServiceName :: Request>

#endif // _ServiceMacro_hpp

#if 0 // example

#include <string>

QuickRPC_0_service_name(abc)
QuickRPC_1_Request_before_fields
int id;
std::string name;
QuickRPC_2_Request_fields_serialized(id,name)
QuickRPC_3_Response_before_fields
// int _status_code;//optional
double score;
QuickRPC_4_Response_fields_serialized(score)

#endif //example
