cmake_minimum_required(VERSION 3.0.0)
project(TinyRPC VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
# include_directories(../TinyTCPServer2/include/)
# include_directories(../../USCiLab/cereal/include/)

file(GLOB_RECURSE SOURCE_CPP ./src/*.cpp)
file(GLOB_RECURSE SOURCE_C ./src/*.c)

find_package(Threads)

add_library(TinyRPCServer SHARED ${SOURCE_CPP} ${SOURCE_C})
target_link_directories(
  TinyRPCServer
  PUBLIC ../TinyTCPServer2/build/
  PUBLIC ../TinyTCPClient/build/
)
target_link_libraries(TinyRPCServer TinyTCPServer2 TinyHTTPServer TinyTCPClient Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
