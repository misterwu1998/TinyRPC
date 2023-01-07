#if !defined(_TinyRPC_QuickClient_hpp)
#define _TinyRPC_QuickClient_hpp

// #include "TinyRPC/service_example/IntToString.hpp"//for coding

#include <sstream>
#include "TinyRPC/client/ConnectionPool.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"

#define TinyRPC_error_fromSerial -9527
#define TinyRPC_error_serverNotConnected -1
#define TinyRPC_error_badRequest -6325
#define TinyRPC_error_badResponse -6326
#define TinyRPC_OK 0

#define TinyRPC_QuickClient(ServiceName) \
int ServiceName::Do(ServiceName::Request const& req, ServiceName::Response& res){ \
  int ret; \
   \
  /*序列化RPC请求*/ \
  Buffer b; \
  serial_Archiver ao(b, serial_ArchiverType::out_binary_littleEndian); \
  ret = ao(req); \
  if(0>ret) return -9527; \
 \
  /*塞进HTTP请求*/ \
  HTTPRequest hreq; \
  hreq.set(http_method::HTTP_POST) \
      .set(std::string("/rpc/")+ "ServiceName" ) \
      .append(*b, b.getLength()); \
\
  /*TODO 获取RPC服务端的连接*/\
  auto hc = ConnectionPool::initOrGetPool().getConnection("ServiceName");\
  if(!hc){\
    return -1;\
  }\
 \
  /*发送HTTP请求*/\
  if(0 > hc->send(hreq)){ \
    return -1; \
  } \
 \
  /*接收HTTP响应*/ \
  HTTPResponse hres; \
  if(0 > hc->recv(hres)){ \
    return -1; \
  } \
  if(hres.status!=http_status::HTTP_STATUS_OK){ \
    return -6325; \
  } \
  if(!(hres.body) || hres.body->getLength()==0){ \
    return -6326; \
  } \
 \
  /*反序列化RPC响应*/ \
  serial_Archiver ai(*(hres.body), serial_ArchiverType::in_binary_littleEndian); \
  ret = ai(res); \
  if(0>ret) return -9527; \
   \
  return 0; \
}

#endif // _TinyRPC_QuickClient_hpp

#if 0 //example
TinyRPC_QuickClient(IntToString)
#endif
