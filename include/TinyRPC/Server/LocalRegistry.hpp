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
#include <unordered_set>
#include "TinyRPC/Server/Cerealization.hpp"

namespace TRPCS
{
  /// @brief 继承关系：TCPConnectionFactory <== HTTPHandlerFactory <== LocalRegistry
  /// 当前类不重载任何基类的函数
  class LocalRegistry : public TTCPS2::HTTPHandlerFactory
  {
  public:

    std::unordered_set<std::string> servicesName;

    template<typename Service>
    LocalRegistry& Register(std::string const& serviceName){
      if(servicesName.count(serviceName)>0){
        TTCPS2_LOGGER.info("TRPCS::LocalRegistry::Register(): service {0} duplicated.", serviceName);
        return *this;
      }

      std::string path;
      if(serviceName[0]=='/') path = serviceName;
      else path = '/'+serviceName;
      auto ret = TTCPS2::HTTPHandlerFactory::route(
        http_method::HTTP_POST, path
      , [](std::shared_ptr<TTCPS2::HTTPRequest> req){
        // 取出HTTP请求体的数据
        if(!req){
          TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPRequest> req): req is nullptr");
          return nullptr;
        }
        if(req->body == nullptr || req->body->getLength()<1){
          TTCPS2_LOGGER.warn("TRPCS::LocalRegistry::Register(): [](std::shared_ptr<TTCPS2::HTTPRequest> req): req doesn't contain HTTP body.");
          return nullptr;
        }
        auto rpcReq = std::make_shared<typename Service::Request>();
        {std::stringstream ss;
        ss << *(req->body);
        
        // 反序列化为RPC请求
        deserialize(ss, *rpcReq);}//delete ss
        
        // 调用Service::Do()获取RPC响应
        std::shared_ptr<typename Service::Response> rpcRes = Service::Do(rpcReq);
        if(!rpcRes){//不能完成服务
          // 400
          auto res = std::make_shared<TTCPS2::HTTPResponse>();
          res->set(http_status::HTTP_STATUS_BAD_REQUEST);
          return res;
        }
        
        // 序列化
        std::string s_rpcRes;
        {std::stringstream ss;
        serialize(*rpcRes, ss);
        s_rpcRes = ss.str();}//delete ss

        // HTTP响应
        auto res = std::make_shared<TTCPS2::HTTPResponse>();
        res->set(http_status::HTTP_STATUS_OK)
            .append(s_rpcRes.data(), s_rpcRes.size());
        s_rpcRes.clear();

        return res;
      }      );

      if(0>ret){
        auto msg = "TRPCS::LocalRegistry::Register(): fail to register the service " + serviceName;
        TTCPS2_LOGGER.warn(msg);
        std::cout << msg << std::endl;
      }
      else if(0==ret){
        TTCPS2_LOGGER.info("TRPCS::LocalRegistry::Register(): service {0} duplicated.", serviceName);
      }
      else{
        servicesName.insert(serviceName);
        TTCPS2_LOGGER.trace("TRPCS::LocalRegistry::Register(): the service {0} been registered locally.", serviceName);
      }
      return *this;
    }

  };
  
} // namespace TRPCS

#endif // _LocalRegistry_hpp
