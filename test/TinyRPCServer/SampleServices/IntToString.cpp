#include "SampleServices/IntToString.hpp"
#include <sstream>

QuickRPC_ReturnType(IntToString) IntToString::Do(QuickRPC_ArgType(IntToString) req){
  if(!req) return nullptr;
  std::stringstream ss;
  ss << req->i;
  auto res = std::make_shared<IntToString::Response>();
  ss >> (res->i);
  return res;
}
