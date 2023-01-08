cmake_minimum_required(VERSION 3.0.0)
project(TinyRPCRegistry VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(./include/)
include_directories(../../TinyTCPServer2/include/)
include_directories(../../MyUtils/include/)
include_directories(../../../gabime/spdlog/include/)

file(GLOB_RECURSE SOURCE_CPP ./src/*.cpp)
file(GLOB_RECURSE SOURCE_C ./src/*.c)

add_executable(TinyRPCRegistry ${SOURCE_C} ${SOURCE_CPP})
target_link_directories(TinyRPCRegistry
  PUBLIC ../../TinyTCPServer2/build/)
target_link_libraries(TinyRPCRegistry
  TinyHTTPServer)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
