cmake_minimum_required(VERSION 3.0.0)
project(TinyRPC VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
include_directories(../TinyTCPServer2/include/)
include_directories(../../USCiLab/cereal/include/)

file(GLOB_RECURSE SERVER_SOURCE_CPP ./Server/*.cpp)
file(GLOB_RECURSE SERVER_SOURCE_C ./Server/*.c)

add_library(TinyRPCServer SHARED ${SERVER_SOURCE_CPP} ${SERVER_SOURCE_C})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
