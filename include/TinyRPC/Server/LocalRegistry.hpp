#if !defined(_LocalRegistry_hpp)
#define _LocalRegistry_hpp

#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyTCPServer2/Logger.hpp"
#include <string>
#include <iostream>
#include "TinyHTTPServer/HTTPHandler.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "util/Buffer.hpp"
#include <sstream>
#include "TinyRPC/Server/Cerealization.hpp"

namespace TRPCS
{
  /// @brief 继承关系：TCPConnectionFactory <== HTTPHandlerFactory <== LocalRegistry
  /// 当前类不重载任何基类的函数
  class LocalRegistry : public TTCPS2::HTTPHandlerFactory
  {
  public:

    template<typename Service>
    LocalRegistry& Register(std::string const& serviceName){

      std::string path;
      if(serviceName[0]=='/') path = serviceName;
      else path = '/'+serviceName;
      auto ret = HTTPHandlerFactory::route(
          http_method::HTTP_POST, path
        , [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int{// TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int
            std::shared_ptr<Service::Request> rpcReq = std::make_shared<Service::Request>();//这里就要求Request有默认构造函数
            std::shared_ptr<Service::Response> rpcRes;

            // 取出请求体的数据
            if(!h){
              TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int: h is null!");
              return -1;
            }
            auto requestNow = h->getRequestNow(); if(!requestNow){
              TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int: requestNow is null!");
              return -1;
            }
            auto bodyBuffer = requestNow->body; if(!bodyBuffer){
              TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int: bodyBuffer is null!");
              return -1;
            }
            uint32_t len; auto rp = bodyBuffer->getReadingPtr(bodyBuffer->getLength(), len);
            if(1>len){
              TTCPS2_LOGGER.info("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int: body contains no data.");
              h->newResponse().setResponse(http_status::HTTP_STATUS_BAD_REQUEST)
                              .doRespond();
              while(h->getResponseNow()) h->doRespond();
              return 0;//是客户端的错，无关紧要
            }
            {std::stringstream ss;
            ss << *bodyBuffer;

            // 反序列化，将Request对象交给服务实现，得到响应
            deserialize(ss, *rpcReq);}
            rpcRes = Service::Do(rpcReq);

            if(rpcRes){//成功
              // 构造HTTP响应，响应体是序列化的Response，发回去
              std::stringstream ss;
              serialize(*rpcRes, ss);
              TTCPS2::Buffer temp;
              ss >> temp;
              rp = temp.getReadingPtr(temp.getLength(), len);
              h->newResponse().setResponse(http_status::HTTP_STATUS_OK)
                              .setResponse(rp, temp.getLength())
                              .doRespond();
              while(h->getResponseNow()){
                if(0>h->doRespond()){
                  TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPHandler> h)->int: fail to send response.");
                  return -1;
                }
              }
              return 0;
            }else{//失败
              h->newResponse().setResponse(http_status::HTTP_STATUS_BAD_REQUEST)
                              .doRespond();
              while(h->getResponseNow()) h->doRespond();
              return 0;//是客户端的错，无关紧要
            }
        }
      );

      if(0>ret){
        auto msg = "TRPCS::LocalRegistry::Register: fail to register the service " + serviceName;
        TTCPS2_LOGGER.warn(msg);
        std::cout << msg << std::endl;
      }
      else if(0==ret){
        TTCPS2_LOGGER.info("TRPCS::LocalRegistry::Register: service {0} duplicated.", serviceName);
      }
      else{
        TTCPS2_LOGGER.trace("TRPCS::LocalRegistry::Register: the service {0} been registered locally.", serviceName);
      }
      return *this;
    }

  };
  
} // namespace TRPCS

#endif // _LocalRegistry_hpp
