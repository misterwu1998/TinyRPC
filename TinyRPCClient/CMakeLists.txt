cmake_minimum_required(VERSION 3.0.0)
project(TinyRPCClient VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
include_directories(../TinyRPCService/include/)
include_directories(../service_example/include/)
include_directories(../../MyUtils/include/)
include_directories(../../Serial/include/)
include_directories(../../TinyTCPClient/include/)
include_directories(../../TinyTCPServer2/include/)

add_executable(Test__ ./test/main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
