#include <iostream>
#include <sstream>
#include <mutex>
#include "TinyHTTPServer/TinyHTTPServer.hpp"
#include "TinyHTTPServer/HTTPHandlerFactory.hpp"
#include "TinyHTTPServer/HTTPMessage.hpp"
#include "util/Config.hpp"
#include "util/Time.hpp"
#include "util/ThreadPool.hpp"

int main(int, char**) {
    
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
        std::string, //服务名称
        std::vector<
            std::pair< //某个RPC服务端
                std::string, //RPC服务端IP
                uint16_t //RPC服务端端口
            >
        >
    > registry;
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
                return res;
            }
            auto serviceName = req->header.find("Service-Name")->second;

            {
                std::lock_guard<std::mutex> lg_registry(m_registry);
                if(
                    registry.count(serviceName)<1 ||
                    registry[serviceName].empty()
                ){//目前没有这个服务的提供者
                    res->set(http_status::HTTP_STATUS_OK); //响应没问题，只是暂时没有这个服务
                    return res;
                }
                auto randomNdx = currentTimeMillis() % (registry[serviceName].size());//负载均衡纯随机
                auto& selected = registry[serviceName][randomNdx];
                res->set(http_status::HTTP_STATUS_OK)
                    .set("IP", selected.first);
                std::string sPort;{
                    std::stringstream ss;
                    ss << selected.second;
                    ss >> sPort;
                }
                res->set("port", sPort);

                if(req->header.count("Dead-IP")>0 &&
                   req->header.count("Dead-Port")>0)
                {//客户端声称某个服务端挂了
                    for(auto it = registry[serviceName].begin();
                        registry[serviceName].end()!=it;
                        it++)
                    {
                        uint16_t deadPort;{
                            std::stringstream ss;
                            ss << req->header.find("Dead-Port")->second;
                            ss >> deadPort;
                        }
                        if(deadPort==it->second &&
                           req->header.find("Dead-IP")->second==it->first)
                        {//是要删除的
                            registry[serviceName].erase(it);
                            break;
                        }
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
                return res;
            }
            auto& serverIP = req->header.find("IP")->second;
            uint16_t serverPort;{
                std::stringstream ss;
                ss << req->header.find("port")->second;
                ss >> serverPort;
            }

            std::lock_guard<std::mutex> lg_registry(m_registry);
            for(auto it = req->header.find("Service-Name");
                req->header.end()!=it && it->first=="Service-Name";
                it++)
            {
                auto& serviceName = it->first;
                registry[serviceName].push_back({serverIP,serverPort});
            }
            res->set(http_status::HTTP_STATUS_OK);

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
                return res;
            }
            auto& serverIP = req->header.find("IP")->second;
            uint16_t serverPort;{
                std::stringstream ss;
                ss << req->header.find("port")->second;
                ss >> serverPort;
            }

            std::lock_guard<std::mutex> lg_registry(m_registry);
            for(auto kv = req->header.find("Service-Name");
                req->header.end()!=kv && kv->first=="Service-Name";
                kv++)
            {
                if(registry.count(kv->second)<1) continue;
                auto& servers = registry[kv->second];
                for(auto ii = servers.begin();
                    servers.end()!=ii;
                    ii++)
                {
                    if(ii->first==serverIP &&
                       ii->second==serverPort)
                    {
                        servers.erase(ii);
                        break;
                    }
                }
            }
            res->set(http_status::HTTP_STATUS_OK);

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
