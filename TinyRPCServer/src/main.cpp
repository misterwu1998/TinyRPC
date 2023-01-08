#include <iostream>
#include <sstream>
#include "TinyRPC/server/LocalRegistry.hpp"
#include "TinyTCPServer2/Logger.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPClient/TinyHTTPClient.hpp"
#include "util/Config.hpp"
#include "util/ThreadPool.hpp"

int main(int, char**) {

    // 日志
    TTCPS2_LOGGER.set_level(spdlog::level::level_enum::trace);
    TTCPS2_LOGGER.flush_on(spdlog::level::level_enum::trace);

    // 注册中心的配置
    auto registryConf = loadConfigure("../conf/registry.properties");
    if(
        registryConf.count("IP")<1 ||
        registryConf.count("port")<1 ||
        registryConf["IP"].empty() ||
        registryConf["port"].empty()
    ){
        std::cout << "Fail to load registry's configuration." << std::endl;
        return 0;
    }
    uint16_t registryPort;{
        std::stringstream ss;
        ss << registryConf["port"];
        ss >> registryPort;
    }

    // 本端的配置
    auto serverConf = loadConfigure("../conf/server.properties");
    if(
        serverConf.count("IP")<1 ||
        serverConf.count("port")<1 ||
        serverConf["IP"].empty() ||
        serverConf["port"].empty()
    ){
        std::cout << "Fail to load local configuration." << std::endl;
        return 0;
    }
    uint16_t serverPort;{
        std::stringstream ss;
        ss << serverConf["port"];
        ss >> serverPort;
    }

    // 本端实现了的RPC服务在进入main()前已经挂载到LocalRegistry，现在需要告知注册中心本端实现的服务
    auto& callbacks = LocalRegistry::getCallbackMap();
    HTTPRequest toRegistry;
    toRegistry.set(http_method::HTTP_GET)
              .set("/register")
              .set("IP", serverConf["IP"])
              .set("port", serverConf["port"]);
    for(auto& kv: callbacks){
        toRegistry.set("Service-Name", kv.first);
    }
    {
        TinyHTTPClient withRegistry(
            registryConf["IP"].c_str(),
            registryPort
        );
        HTTPResponse fromRegistry;
        if(
            0>withRegistry.send(toRegistry) ||
            0>withRegistry.recv(fromRegistry)
        ){
            std::cout << "Fail to communicate with registry." << std::endl;
            return 0;
        }
        if(fromRegistry.status!=http_status::HTTP_STATUS_OK){
            std::cout << "Registry didn't feel OK." << std::endl;
            return 0;
        }
    }

    // 启动HTTP服务端
    auto setting = std::make_shared<HTTPHandlerFactory>();
    setting->route(
        [](std::shared_ptr<HTTPRequest> req){
            return req && 
                   req->url.length() > sizeof("/rpc/")-1 &&
                   req->url.substr(
                    0, sizeof("/rpc/")-1
                   )=="/rpc/";
        },
        [callbacks](
            std::shared_ptr<HTTPRequest> req
        )-> std::shared_ptr<HTTPResponse> {
            
            auto serviceName = req->url.substr(
                sizeof("/rpc/")-1
            );
            TTCPS2_LOGGER.trace(
                "[lambda url='/rpc/'] Client needs service {0}.",
                serviceName
            );

            auto it = callbacks.find(serviceName);
            if(callbacks.end()==it){
                TTCPS2_LOGGER.info(
                    "[lambda url='/rpc/'] Client asks for unimplemented service."
                );
                return nullptr;
            }
            auto func = it->second;
            TTCPS2_LOGGER.trace(
                "[lambda url='/rpc/'] The service {0} is implemented.",
                serviceName
            );
            return func(req);
        }
    );
    TinyHTTPServer server(
        serverConf["IP"].c_str(), serverPort,
        128, 2, setting, &(ThreadPool::getPool(2))
    );
    if(false){//是否仅和注册中心联调
        std::cout << "Input something to shutdown: ";
        getchar();
    }else{
        server.run();
        std::cout << "Input something to shutdown: ";
        getchar();
        server.shutdown();
    }

    // 即将结束服务，需要告知注册中心
    toRegistry.set(http_method::HTTP_GET)
              .set("/drop")
              .set("IP", serverConf["IP"])
              .set("port", serverConf["port"]);
    for(auto& kv: callbacks){
        toRegistry.set("Service-Name", kv.first);
    }
    {
        TinyHTTPClient withRegistry(
            registryConf["IP"].c_str(),
            registryPort
        );
        HTTPResponse fromRegistry;
        if(
            0>withRegistry.send(toRegistry) ||
            0>withRegistry.recv(fromRegistry)
        ){
            std::cout << "Fail to communicate with registry." << std::endl;
            return 0;
        }
        if(fromRegistry.status!=http_status::HTTP_STATUS_OK){
            std::cout << "Registry didn't feel OK." << std::endl;
            return 0;
        }
    }

    return 0;
}
