#include "TinyRPC/ServiceMacro.hpp"
#include <string>

QuickRPC_0_service_name(IntToString)
QuickRPC_1_Request_before_fields
  int64_t i;
QuickRPC_2_Request_fields_serialized(i)
QuickRPC_3_Response_before_fields
  std::string i;
QuickRPC_4_Response_fields_serialized(i)
