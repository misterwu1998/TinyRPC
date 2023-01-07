#if !defined(_TinyRPC_QuickService_hpp)
#define _TinyRPC_QuickService_hpp

#include "serial/single_include.hpp"

/// 确定RPC服务接口名称；随后请定义RPC请求的具体参数
#define TinyRPC_QuickService_step0_beforeRequestParam(ServiceName)\
class ServiceName {                                               \
public:                                                           \  
  class Request{                                                  \
  public:                                                         

    /*用户定义的请求参数*/

/// 重复列出RPC请求各参数的名称
#define TinyRPC_QuickService_step1_reqParamNames(...)             \
    int serialize(serial_Archiver& a){                            \
      return a(__VA_ARGS__);                                      \
    }

/// 随后请定义RPC响应的具体参数
#define TinyRPC_QuickService_step2_beforeResponseData             \
  };                                                              \
  class Response{                                                 \
  public:

    /*用户定义的响应参数*/

/// 重复列出RPC响应各成员的名称
#define TinyRPC_QuickService_step3_resFieldNames(...)             \
    int serialize(serial_Archiver& a){                            \
      return a(__VA_ARGS__);                                      \
    }

#define TinyRPC_QuickService_step4_end                            \
  };                                                              \
  static int Do(Request const& req, Response& res);               \  
};

#endif /// _TinyRPC_QuickService_hpp

#if 0 ///example

#include <string>

TinyRPC_QuickService_step0_beforeRequestParam(abc)
  int i;
  float f;
TinyRPC_QuickService_step1_reqParamNames(i,f)
TinyRPC_QuickService_step2_beforeResponseData
  int64_t i;
  double d;
TinyRPC_QuickService_step3_resFieldNames(i,d)
TinyRPC_QuickService_step4_end

#endif
