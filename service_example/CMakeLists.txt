cmake_minimum_required(VERSION 3.0.0)
project(TinyRPC_service_example VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
include_directories(../TinyRPCService/include/)
include_directories(../../Serial/include/)
include_directories(../../MyUtils/include/)

add_library(Test__ SHARED ./test.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
