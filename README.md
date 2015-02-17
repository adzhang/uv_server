# uv_server
-------
尝试用简单的C++封装libuv库，学习性质。<br>

# <i class="icon-hdd"></i>介绍:<br>
通过改造libuv自带的echo-server，封装了Session(客户socket)和NetImpl两个模块，所有libuv的实现细节都在这两个模块里面，接口文件在include/net/net.h.<br>

# Session:<br>
对应于每个tcp client socket，目前实现了处理网络消息接收，分配接收缓冲区Buffer(简单的定长环形队列），分包消息处理(len:body)的网络协议，当len为0时，用来作为当前的echo-server实现。<br>

# NetImpl:<br>
负责起服务端口，消息loop和新消息connection响应。每次connection会新建一个Session，Session加入到事件响应后接管client socket。<br>

E-mail: 694164705@qq.com<br>
Github: https://github.com/adzhang<br>
