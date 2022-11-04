#include "TinyRPC/Server/TinyRPCServer.hpp"
#include "TinyRPC/Server/LocalRegistry.hpp"
#include "util/ThreadPool.hpp"

#include "SampleServices/IntToString.hpp"

int main(int argc, char const *argv[])
{
  auto lr = std::make_shared<TRPCS::LocalRegistry>();
  lr->Register<IntToString>("IntToString");
  TRPCS::TinyRPCServer s(
    "127.0.0.1", 6325, 16, 2
  , &(TTCPS2::ThreadPool::getPool(2))
  , lr  );
  s.run();
  std::cout << "Input something to shutdown: ";
  getchar();
  s.shutdown();

  return 0;
}
