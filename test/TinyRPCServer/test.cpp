#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyRPC/Server/LocalRegistry.hpp"
#include "util/ThreadPool.hpp"
#include "util/Config.hpp"

#include "SampleServices/IntToString.hpp"

int main(int argc, char const *argv[])
{
  auto lr = std::make_shared<TRPCS::LocalRegistry>();
  lr->Register<IntToString>("IntToString");
  
  auto conf = TTCPS2::loadConfigure("../conf/Server/rpcServer.properties");
  if(conf.count("IP")<1 || conf.count("port")<1){
    std::cout << "Fail to load IP or port from ../conf/Server/rpcServer.properties" << std::endl;
    exit(-1);
  }
  auto ip = conf["IP"].c_str();
  unsigned short port; {std::stringstream ss; ss << conf["port"]; ss >> port;}
  TRPCS::TinyRPCServer s(conf["IP"].c_str(), port, 16, 2, &(TTCPS2::ThreadPool::getPool(2)), lr  );

  s.run();
  std::cout << "Input something to shutdown: ";
  getchar();
  s.shutdown();

  return 0;
}
