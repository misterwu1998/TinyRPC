#include "TinyRPC/service_example/IntToString.hpp"
#include "TinyRPC/client/QuickClient.hpp"

#define _DEBUGGING 1

#if _DEBUGGING
int IntToString::Do(IntToString::Request const &req, IntToString::Response &res)
{
  int ret;
  Buffer b;
  serial_Archiver ao(b, serial_ArchiverType::out_binary_littleEndian);
  ret = ao(req);
  if (0 > ret)
    return -9527;
  HTTPRequest hreq;
  hreq.set(http_method::HTTP_POST).set(std::string("/rpc/") + "IntToString").append(*b, b.getLength());
  std::shared_ptr<TinyHTTPClient> dead(nullptr);
  for (uint16_t t = TinyRPC_ClientRetries::times; t >= 0; t--)
  {
    auto hc = TinyRPCClient_ConnectionPool::initOrGetPool().getConnection("IntToString", dead);
    if (!hc)
    {
      return -1;
    }
    if (0 > hc->send(hreq))
    {
      dead = hc;
      continue;
    }
    HTTPResponse hres;
    if (0 > hc->recv(hres))
    {
      dead = hc;
      continue;
    }
    if (hres.status != http_status::HTTP_STATUS_OK)
    {
      return -6325;
    }
    if (!(hres.body) || hres.body->getLength() == 0)
    {
      return -6326;
    }
    serial_Archiver ai(*(hres.body), serial_ArchiverType::in_binary_littleEndian);
    ret = ai(res);
    if (0 > ret)
      return -9527;
    return 0;
  }
  return -1;
}
#else  //_DEBUGGING
TinyRPC_QuickClient(IntToString)
#endif //_DEBUGGING
