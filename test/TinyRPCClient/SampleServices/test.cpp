#include <iostream>
#include "TinyRPC/Client/RPCClientMacro.hpp"

#include "SampleServices/IntToString.hpp"
#if 0 // debug
std::shared_ptr<IntToString ::Response> IntToString ::Do(std::shared_ptr<IntToString ::Request> req)
{
  if (!req)
    return nullptr;
  auto &pool = TRPC::RPCClientPool::getPool();
  auto conn = pool.get("IntToString");
  if (!conn)
  {
    auto reqToRegistry = std::make_shared<TTCPS2::HTTPRequest>();
    reqToRegistry->set(http_method::HTTP_GET).set("/find").set("RPC-Service-Name", "IntToString");
    auto registryConf = TTCPS2::loadConfigure("../conf/TinyRPCClient/registry.properties");
    ushort pp;
    {
      std::stringstream ss;
      ss << registryConf["RegistryPort"];
      ss >> pp;
    }
    ::TinyHTTPClient c(registryConf["RegistryIP"].c_str(), pp);
    if (0 > c.send(*reqToRegistry))
    {
      TTCPS2_LOGGER.error(std::string("IntToString") + "::Do(): fail to send to Registry at {0}:{1}.", registryConf["RegistryIP"], registryConf["RegistryPort"]);
      exit(-1);
    }
    auto resFromRegistry = std::make_shared<TTCPS2::HTTPResponse>();
    if (0 > c.recv(*resFromRegistry))
    {
      TTCPS2_LOGGER.error(std::string("IntToString") + "::Do(): fail to recv from Registry at {0}:{1}.", registryConf["RegistryIP"], registryConf["RegistryPort"]);
      exit(-1);
    }
    if (resFromRegistry->status != http_status::HTTP_STATUS_OK || resFromRegistry->header.count("RPC-Server-IP") < 1 || resFromRegistry->header.count("RPC-Server-Port") < 1)
    {
      return nullptr;
    }
    std::string ip, port;
    ip = resFromRegistry->header.find("RPC-Server-IP")->second;
    port = resFromRegistry->header.find("RPC-Server-Port")->second;
    {
      std::stringstream ss;
      ss << port;
      ss >> pp;
    }
    conn = std::make_shared<::TinyHTTPClient>(ip.c_str(), pp);
  }
  std::string seri;
  {
    std::stringstream ss;
    TRPC::serialize(*req, ss);
    seri = ss.str();
  }
  auto httpReq = std::make_shared<TTCPS2::HTTPRequest>();
  auto httpRes = std::make_shared<TTCPS2::HTTPResponse>();
  httpReq->set(http_method::HTTP_POST).set(std::string("/") + "IntToString").append(seri.data(), seri.size());
  if (0 > conn->send(*httpReq) || 0 > conn->recv(*httpRes))
  {
    TTCPS2_LOGGER.warn(std::string("IntToString") + "::Do(): 0 > conn->send(*httpReq) || 0 > conn->recv(*httpRes)");
    auto reqToRegistry = std::make_shared<TTCPS2::HTTPRequest>();
    reqToRegistry->set(http_method::HTTP_GET).set("/find").set("RPC-Service-Name", "IntToString").set("RPC-Server-IP", conn->getServerIP()).set("RPC-Server-Port", std::to_string(conn->getServerPort()));
    auto registryConf = TTCPS2::loadConfigure("../conf/TinyRPCClient/registry.properties");
    ushort pp;
    {
      std::stringstream ss;
      ss << registryConf["RegistryPort"];
      ss >> pp;
    }
    ::TinyHTTPClient c(registryConf["RegistryIP"].c_str(), pp);
    if (0 > c.send(*reqToRegistry))
    {
      TTCPS2_LOGGER.error(std::string("IntToString") + "::Do(): fail to send to Registry at {0}:{1}.", registryConf["RegistryIP"], registryConf["RegistryPort"]);
      exit(-1);
    }
    auto resFromRegistry = std::make_shared<TTCPS2::HTTPResponse>();
    if (0 > c.recv(*resFromRegistry))
    {
      TTCPS2_LOGGER.error(std::string("IntToString") + "::Do(): fail to recv from Registry at {0}:{1}.", registryConf["RegistryIP"], registryConf["RegistryPort"]);
      exit(-1);
    }
    if (resFromRegistry->status != http_status::HTTP_STATUS_OK || resFromRegistry->header.count("RPC-Server-IP") < 1 || resFromRegistry->header.count("RPC-Server-Port") < 1)
    {
      return nullptr;
    }
    return nullptr;
  }
  else
  {
    TTCPS2_LOGGER.trace(std::string("IntToString") + "::Do(): RPC server available.");
    pool.put("IntToString", conn);
  }
  if (httpRes->body == nullptr || httpRes->body->getLength() < 1)
  {
    return nullptr;
  }
  auto res = std::make_shared<IntToString ::Response>();
  {
    std::stringstream ss;
    ss << *(httpRes->body);
    TRPC::deserialize(ss, *res);
  }
  return res;
}
#else
QUICK_RPC_CLIENT(IntToString)
#endif
int main(int argc, char const *argv[])
{
  auto rpcReq = std::make_shared<IntToString::Request>();
  rpcReq->i = 114514;
  auto rpcRes = IntToString::Do(rpcReq);
  std::cout << rpcRes->i;
  return 0;
}
