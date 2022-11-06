#include "SampleServices/IntToString.hpp"
#include <sstream>

QUICK_RPC_RETURN_TYPE(IntToString) IntToString::Do(QUICK_RPC_ARG_TYPE(IntToString) req){
  if(!req) return nullptr;
  std::stringstream ss;
  ss << req->i;
  auto res = std::make_shared<IntToString::Response>();
  ss >> (res->i);
  return res;
}
