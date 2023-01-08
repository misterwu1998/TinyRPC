#include <iostream>
#include <sstream>
#include <mutex>
#include "TinyTCPServer2/Logger.hpp"
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "util/Config.hpp"
#include "util/Time.hpp"
#include "util/ThreadPool.hpp"

int main(int, char**) {

    // 日志
    TTCPS2_LOGGER.set_level(spdlog::level::level_enum::trace);
    TTCPS2_LOGGER.flush_on(spdlog::level::level_enum::trace);
    
    // 本端的配置
    auto registryConf = loadConfigure("../conf/registry.properties");
    if(
        registryConf.count("IP")<1 ||
        registryConf.count("port")<1 ||
        registryConf["IP"].empty() ||
        registryConf["port"].empty()
    ){
        std::cout << "Fail to load local configuration." << std::endl;
        return 0;
    }
    uint16_t registryPort;{
        std::stringstream ss;
        ss << registryConf["port"];
        ss >> registryPort;
    }

    std::unordered_map<
        std::string,/*服务名称*/
        std::vector<
            std::pair/*某个RPC服务端*/<
                std::string,/*IP*/
                uint16_t/*端口*/>>> registry;
    std::mutex m_registry;

    auto settings = std::make_shared<HTTPHandlerFactory>();

    settings->route(
        [](std::shared_ptr<HTTPRequest> req){
            return req && req->url=="/find"; //客户端来查询了
        },
        [&registry, &m_registry](
            std::shared_ptr<HTTPRequest> req
        ){
            auto res = std::make_shared<HTTPResponse>();

            if(req->header.count("Service-Name")<1){
                res->set(http_status::HTTP_STATUS_BAD_REQUEST);
                TTCPS2_LOGGER.info("[lambda url=='/find'] Bad request.");
                return res;
            }
            auto serviceName = req->header.find("Service-Name")->second;
            TTCPS2_LOGGER.trace("[lambda url=='/find'] Service-Name: {0}", serviceName);

            std::lock_guard<std::mutex> lg_registry(m_registry);
            if( registry.count(serviceName)<1 ||
                registry[serviceName].empty()
            ){//目前没有这个服务的提供者
                res->set(http_status::HTTP_STATUS_OK); //响应没问题，只是暂时没有这个服务
                TTCPS2_LOGGER.trace(
                    "[lambda url=='/find'] No server of service {0} yet.",
                    serviceName);
                return res;
            }
            auto& servers = registry[serviceName];
            auto randomNdx = currentTimeMillis() % (servers.size());//负载均衡纯随机
            auto& selected = servers[randomNdx];
            std::string sPort;{
                std::stringstream ss;
                ss << selected.second;
                ss >> sPort;
            }
            res->set(http_status::HTTP_STATUS_OK)
                .set("IP", selected.first)
                .set("port", sPort);
            TTCPS2_LOGGER.trace(
                "[lambda url=='/find'] Selected server is at {0}:{1}",
                selected.first, selected.second);

            if( req->header.count("Dead-IP")>0 &&
                req->header.count("Dead-Port")>0)
            {//客户端声称某个服务端挂了
                auto& deadIP = req->header.find("Dead-IP")->second;
                uint16_t deadPort;{
                    std::stringstream ss;
                    ss << req->header.find("Dead-Port")->second;
                    ss >> deadPort;
                }
                TTCPS2_LOGGER.trace(
                    "[lambda url=='/find'] Client said {0}:{1} is dead.",
                    deadIP, deadPort
                );

                for(auto sv = servers.begin();
                    servers.end()!=sv;
                    sv++)
                {
                    if( deadPort==sv->second &&
                        deadIP==sv->first)
                    {//是要删除的
                        servers.erase(sv);
                        TTCPS2_LOGGER.trace(
                            "[lambda url=='/find'] The server that is dead said by client has been deleted."
                        );
                        break;
                    }
                }
            }

            return res;
        }
    );

    settings->route(
        [](std::shared_ptr<HTTPRequest> req){
            return req && req->url=="/register";//服务端来注册了
        },
        [&registry, &m_registry](
            std::shared_ptr<HTTPRequest> req
        ){
            auto res = std::make_shared<HTTPResponse>();

            if(req->header.count("IP")<1 ||
               req->header.count("port")<1)
            {
                res->set(http_status::HTTP_STATUS_BAD_REQUEST);
                TTCPS2_LOGGER.info("[lambda url=='/register'] Bad request.");
                return res;
            }
            auto& serverIP = req->header.find("IP")->second;
            uint16_t serverPort;{
                std::stringstream ss;
                ss << req->header.find("port")->second;
                ss >> serverPort;
            }
            TTCPS2_LOGGER.trace(
                "[lambda url=='/register'] Server is at {0}:{1}.",
                serverIP, serverPort
            );

            std::lock_guard<std::mutex> lg_registry(m_registry);
            for(auto kv_serviceName = req->header.find("Service-Name");
                req->header.end()!=kv_serviceName && kv_serviceName->first=="Service-Name";
                kv_serviceName++)
            {
                auto& serviceName = kv_serviceName->second;
                auto& servers = registry[serviceName];
                auto flag = false;
                for(auto& sv: servers){
                    if(sv.first==serverIP &&
                       sv.second==serverPort)//此前有过注册且还为注销，不能有重复
                    {
                        flag = true;                        
                        break;
                    }
                }
                if(flag)//此前有过注册且还为注销，不能有重复
                {
                    TTCPS2_LOGGER.trace("[lambda url=='/register'] Done before.");
                    break;
                }
                servers.push_back({serverIP,serverPort});
            }
            res->set(http_status::HTTP_STATUS_OK);
            TTCPS2_LOGGER.trace("[lambda url=='/register'] OK");

            return res;
        }
    );

    settings->route(
        [](std::shared_ptr<HTTPRequest> req){
            return req && req->url=="/drop";//服务端来注销了
        },
        [&registry, &m_registry](
            std::shared_ptr<HTTPRequest> req
        ){
            auto res = std::make_shared<HTTPResponse>();

            if(req->header.count("IP")<1 ||
               req->header.count("port")<1)
            {
                res->set(http_status::HTTP_STATUS_BAD_REQUEST);
                TTCPS2_LOGGER.info("[lambda url=='/drop'] Bad request.");
                return res;
            }
            auto& serverIP = req->header.find("IP")->second;
            uint16_t serverPort;{
                std::stringstream ss;
                ss << req->header.find("port")->second;
                ss >> serverPort;
            }
            TTCPS2_LOGGER.trace(
                "[lambda url=='/drop'] Server is at {0}:{1}.",
                serverIP,serverPort
            );

            std::lock_guard<std::mutex> lg_registry(m_registry);
            for(auto kv_serviceName = req->header.find("Service-Name");
                req->header.end()!=kv_serviceName && kv_serviceName->first=="Service-Name";
                kv_serviceName++)
            {
                auto& serviceName = kv_serviceName->second;
                if(registry.count(serviceName)<1) continue;
                auto& servers = registry[serviceName];
                for(auto sv = servers.begin();
                    servers.end()!=sv;
                    sv++)
                {
                    if( sv->first==serverIP &&
                        sv->second==serverPort)
                    {
                        servers.erase(sv);
                        TTCPS2_LOGGER.trace(
                            "[lambda url=='/drop'] Server at {0}:{1} drops service {2}.",
                            serverIP,serverPort, serviceName
                        );
                        break;
                    }
                }
            }
            res->set(http_status::HTTP_STATUS_OK);
            TTCPS2_LOGGER.trace("[lambda url=='/drop'] OK");

            return res;
        }
    );

    TinyHTTPServer server(
        registryConf["IP"].c_str(), registryPort,
        128, 2, settings,
        &(ThreadPool::getPool(2))
    );
    server.run();
    std::cout << "Input something to shutdown: ";
    getchar();
    server.shutdown();
    
    return 0;
}
