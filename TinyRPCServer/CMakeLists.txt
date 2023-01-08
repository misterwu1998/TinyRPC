cmake_minimum_required(VERSION 3.0.0)
project(TinyRPCServer VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
include_directories(../TinyRPCService/include/)
include_directories(../service_example/include/)
include_directories(../../TinyTCPServer2/include/)
include_directories(../../Serial/include/)
include_directories(../../MyUtils/include/)
include_directories(../../TinyTCPClient/include/)
include_directories(../../../gabime/spdlog/include/)

file(GLOB_RECURSE SOURCE_CPP ./src/**.cpp)
file(GLOB_RECURSE SOURCE_C ./src/**.c)

add_executable(TinyRPCServer ${SOURCE_CPP} ${SOURCE_C})
target_link_directories(TinyRPCServer
  PUBLIC ../../TinyTCPServer2/build/
  PUBLIC ../../TinyTCPClient/build/)
target_link_libraries(TinyRPCServer
  TinyHTTPServer
  TinyHTTPClient)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
