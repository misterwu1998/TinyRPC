#include <iostream>
#include "TinyRPC/client/ConnectionPool.hpp"
#include "TinyRPC/service_example/IntToString.hpp"

int main(int, char**) {
    // 加载注册中心的配置
    TinyRPCClient_ConnectionPool::initOrGetPool("../conf/registry.properties");

    IntToString::Request req = {114514};
    IntToString::Response res;
    auto ret = IntToString::Do(req,res);
    if(0<=ret) std::cout << res.s;
    return ret;
}
