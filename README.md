- 依赖
   1. [TinyTCPServer2、TinyHTTPServer](github.com/misterwu1998/TinyTCPServer2/)
   2. [TinyTCPClient、TinyHTTPClient](github.com/misterwu1998/TinyTCPClient/)
- 简单的目录结构
  - [配置文件](./conf/)<br>相对于工作目录，默认是```../conf/```
    - [客户端](./conf/TinyRPCClient/)
      - [注册中心的属性](./conf/TinyRPCClient/registry.properties)
        - RegistryIP
        - RegistryPort
        - 其它键值对
      - 客户端用得到的其它配置~~
    - [注册中心](./conf/TinyRPCRegistry/)
      - [注册中心自己的属性](./conf/TinyRPCClient/registry.properties)
        - RegistryIP
        - RegistryPort
        - 其它键值对
      - 注册中心用得到的其它配置~~
    - [服务端](./conf/TinyRPCServer/)
      - [注册中心的属性](./conf/TinyRPCClient/registry.properties)
        - RegistryIP
        - RegistryPort
        - 其它键值对
      - [服务端自己的属性](./conf/TinyRPCServer/)
        <br>*服务端的配置文件这么放可能不妥，单机无法启动多个服务端，因为端口撞一块了……先不管了*
        - IP
        - port
        - 其它键值对
      - 服务端用得到的其它配置~~
  - [接口声明文件的示例](./include/SampleServices/)
    <br>就是演示怎么用[宏](./include/TinyRPC/ServiceMacro.hpp)快捷定义一个RPC接口
  - [头文件](./include/)
    - [TinyRPC](./include/TinyRPC/)
      - [客户端](./include/TinyRPC/Client/)
        - [客户端快速实现服务的宏](./include/TinyRPC/Client/RPCClientMacro.hpp)
          <br>包含了想调用的接口的声明文件后，以接口名为参数展开宏定义就OK了
        - [客户端简单的连接池](./include/TinyRPC/Client/RPCClientPool.hpp)似乎用不上，不该放在这里的？先不管了
      - [注册中心](./include/TinyRPC/Registry/)
        <br>就是[一个简单的HTTP服务端](./include/TinyRPC/Registry/TinyRPCRegistry.hpp)
      - [服务端](./include/TinyRPC/Server/)
        - [本地注册中心](./include/TinyRPC/Server/LocalRegistry.hpp)
          <br>核心就是个函数模板，代码膨胀机，每挂一个服务就膨胀一份。包含了想实现的接口的声明文件后，`main()`手动把它挂上。
        - [服务器](./include/TinyRPC/Server/TinyRPCServer.hpp)
          <br>创建的时候，把刚才那个本地注册中心交给服务器
      - [序列化协议](./include/TinyRPC/Cerealization.hpp)
        <br>白嫖了[cereal](github.com/USCiLab/cereal/)，build出库文件前确定用二进制、JSON还是XML
      - [RPC服务的快捷宏定义](./include/TinyRPC/ServiceMacro.hpp)
        <br>0~4按顺序使用即可。展开出来的`Do()`在客户端、服务端会有不同的实现，客户端用上述的[客户端快速实现服务的宏](./include/TinyRPC/Client/RPCClientMacro.hpp)实现，服务端则要自定义，真正实现这个服务
