#include "TinyRPC/Registry/TinyRPCRegistry.hpp"
#include "util/Config.hpp"
#include "util/ThreadPool.hpp"
#include "TinyTCPServer2/Logger.hpp"
#include <sstream>
#include <iostream>

int main(int argc, char const *argv[])
{
  auto spdLogger = spdlog::rotating_logger_mt(
      "TinyRPCRegistry"
    , "./temp/log/TinyRPCRegistry/log"
    , 1024*1024*4, 4);
  spdLogger->set_pattern("[%Y%m%d %H:%M:%S.%e %z][%l][thread %t] %v");
  spdLogger->set_level(spdlog::level::level_enum::trace);
  spdLogger->flush_on(spdlog::level::level_enum::info);
  TTCPS2::Logger::initOrGet(spdLogger);

  auto conf = TTCPS2::loadConfigure("../conf/TinyRPCRegistry/registry.properties");
  unsigned short port; { std::stringstream ss; ss << conf["RegistryPort"]; ss >> port; }
  TRPC::TinyRPCRegistry r(conf["RegistryIP"].c_str(), port, 8, 2, &(TTCPS2::ThreadPool::getPool(2)));
  r.run();
  std::cout << "Input something to shutdown: ";
  getchar();
  r.shutdown();
  return 0;
}
