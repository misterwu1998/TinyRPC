#include <sstream>
#include "TinyRPC/service_example/IntToString.hpp"
#include "TinyRPC/server/QuickImpl.hpp"

TinyRPC_Do_returnZeroIfOK(IntToString,req,res){
  std::stringstream ss;
  ss << req.i;
  ss >> res.s;
  return 0;
}

TinyRPC_register_locally(IntToString)
