#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyRPC/Server/LocalRegistry.hpp"
#include "util/ThreadPool.hpp"
#include "util/Config.hpp"

#include "SampleServices/IntToString.hpp"

int main(int argc, char const *argv[])
{
  auto spdLogger = spdlog::rotating_logger_mt(
      "TinyRPCServer"
    , "./temp/log/TinyRPCServer/log"
    , 1024*1024*4, 4);
  spdLogger->set_pattern("[%Y%m%d %H:%M:%S.%e %z][%l][thread %t] %v");
  spdLogger->set_level(spdlog::level::level_enum::trace);
  spdLogger->flush_on(spdlog::level::level_enum::info);
  TTCPS2::Logger::initOrGet(spdLogger);

  auto lr = std::make_shared<TRPC::LocalRegistry>();
  lr->Register<IntToString>("IntToString");
  
  auto conf = TTCPS2::loadConfigure("../conf/TinyRPCServer/rpcServer.properties");
  if(conf.count("IP")<1 || conf.count("port")<1){
    std::cout << "Fail to load IP or port from ../conf/TinyRPCServer/rpcServer.properties" << std::endl;
    exit(-1);
  }
  auto ip = conf["IP"].c_str();
  unsigned short port; {std::stringstream ss; ss << conf["port"]; ss >> port;}
  TRPC::TinyRPCServer s(conf["IP"].c_str(), port, 16, 2, &(TTCPS2::ThreadPool::getPool(2)), lr  );

  s.run();
  std::cout << "Input something to shutdown: ";
  getchar();
  s.shutdown();

  return 0;
}
