#if !defined(_TinyRPC_QuickImpl_hpp)
#define _TinyRPC_QuickImpl_hpp

#include "TinyRPC/server/LocalRegistry.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "serial/single_include.hpp"

#define TinyRPC_Do_returnZeroIfOK(ServiceName, requestArgName, responseArgName) \
int ServiceName::Do( \
  ServiceName::Request const& requestArgName, \
  ServiceName::Response& responseArgName \
)

#if 0 // for coding
#include "TinyRPC/service_example/IntToString.hpp"
TinyRPC_Do_returnZeroIfOK(IntToString,a,b){}
#endif // for coding

#define TinyRPC_register_locally(ServiceName)\
std::shared_ptr<HTTPResponse> _TinyRPC_callback_##ServiceName(\
  std::shared_ptr<HTTPRequest> req\
){\
  if(!req) return nullptr;\
  auto res = std::make_shared<HTTPResponse>();\
\
  /* 反序列化RPC请求 */\
  if(!(req->body) || req->body->getLength()==0){\
    /* bad request */\
    res->set(http_status::HTTP_STATUS_BAD_REQUEST);\
    return res;\
  }\
  ServiceName::Request rpcreq;\
  serial_Archiver ai(*(req->body), serial_ArchiverType::in_binary_littleEndian);\
  if(0>ai(rpcreq)){\
    return nullptr;\
  }\
\
  /* RPC响应 */\
  ServiceName::Response rpcres;\
  if(0 > ServiceName::Do(rpcreq,rpcres)){\
    /* bad request */\
    res->set(http_status::HTTP_STATUS_BAD_REQUEST);\
    return res;\
  }\
\
  /* 序列化RPC响应 */\
  {\
    Buffer b;\
    serial_Archiver ao(b, serial_ArchiverType::out_binary_littleEndian);\
    if(0>ao(rpcres)){\
      return nullptr;\
    }\
\
  /* HTTP响应 */\
    res->set(http_status::HTTP_STATUS_OK)\
        .append((char*)*b, b.getLength());\
  }\
  return res;\
}\
\
LocalRegistryAction _TinyRPC_register_##ServiceName(\
  #ServiceName , _TinyRPC_callback_##ServiceName\
);

#endif // _TinyRPC_QuickImpl_hpp
