cmake_minimum_required(VERSION 3.0.0)
project(TinyRPC VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Threads)

include_directories(./include/)
# include_directories(../TinyTCPServer2/include/)
# include_directories(../../USCiLab/cereal/include/)

file(GLOB_RECURSE SERVER_SOURCE_CPP ./src/TinyRPC/Server/*.cpp)
file(GLOB_RECURSE SERVER_SOURCE_C ./src/TinyRPC/Server/*.c)
add_library(TinyRPCServer SHARED ${SERVER_SOURCE_CPP} ${SERVER_SOURCE_C})
target_link_directories(
  TinyRPCServer
  PUBLIC ../TinyTCPServer2/build/
  PUBLIC ../TinyTCPClient/build/
)
target_link_libraries(TinyRPCServer TinyTCPServer2 TinyHTTPServer TinyHTTPClient TinyTCPClient Threads::Threads)

file(GLOB_RECURSE REGISTRY_SOURCE_CPP ./src/TinyRPC/Registry/*.cpp)
add_library(TinyRPCRegistry SHARED ${REGISTRY_SOURCE_CPP})
target_link_directories(
  TinyRPCRegistry
  PUBLIC ../TinyTCPServer2/build/
  PUBLIC ../TinyTCPClient/build/
)
target_link_libraries(TinyRPCRegistry TinyTCPServer2 TinyHTTPServer TinyHTTPClient TinyTCPClient Threads::Threads)

file(GLOB_RECURSE TEST_SERVER_CPP ./test/TinyRPCServer/*.cpp)
add_executable(testTinyRPCServer ${TEST_SERVER_CPP})
target_link_directories(
  testTinyRPCServer
  PUBLIC ../TinyTCPServer2/build/
  PUBLIC ../TinyTCPClient/build/
  PUBLIC ./build/
)
target_link_libraries(testTinyRPCServer TinyRPCServer TinyTCPServer2 TinyHTTPClient TinyHTTPServer TinyTCPClient Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
