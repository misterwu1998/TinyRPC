#if !defined(_IntToString_hpp)
#define _IntToString_hpp

#include <string>
#include "TinyRPC/QuickService.hpp"

TinyRPC_QuickService_step0_beforeRequestParam(IntToString)
  int i;
TinyRPC_QuickService_step1_reqParamNames(i)
TinyRPC_QuickService_step2_beforeResponseData
  std::string s;
TinyRPC_QuickService_step3_resFieldNames(s)
TinyRPC_QuickService_step4_end

#endif // _IntToString_hpp
