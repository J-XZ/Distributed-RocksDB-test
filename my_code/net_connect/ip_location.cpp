//
// Created by jxz on 2022/2/14.
//

#include "ip_location.h"


Ip_location::Ip_location(const string &ip_address, int port) {
    store_node_address.sin_family = AF_INET;
    store_node_address.sin_port = htons(port);
    // inet_pton
    // 从 cp 开始的缓冲区中的 Internet 号码的表示格式转换为二进制网络格式，并将接口类型 af 的结果存储在以 buf 开始的缓冲区中。
    if (inet_pton(AF_INET, ip_address.c_str(), &store_node_address.sin_addr) <= 0) {
        printf("inet_pton error for %s\n", ip_address.c_str());
    }
}

int Ip_location::connect_to() {
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("创建socket失败，%s(errno: %d)\n", strerror(errno), errno);
    }
    if (connect(socket_fd, (struct sockaddr *) &store_node_address, sizeof(store_node_address)) < 0) {
        printf("connect_to error: %s(errno: %d)\n", strerror(errno), errno);
    }
    return socket_fd;
}

int Ip_location::receive_from(int port) {
    int listen_fd, connect_fd;
    // 使用协议 protocol 在域 domain 中创建 type 类型的新套接字。
    // 如果 protocol 为零，则自动选择一个。
    // 返回新套接字的文件描述符，或 -1 表示错误。
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
    }
    sockaddr_in listen_address{};

    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *) &opt, sizeof(opt))) {
        perror("set_socket_opt");
        return -1;
    }

    listen_address.sin_family = AF_INET;
    listen_address.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_address.sin_port = htons(port);
    // 当使用 socket 函数创建套接字时，它存在于名称空间（地址族）中，但没有分配给它的地址。
    // bind() 将 addr 指定的地址分配给文件描述符 fd 引用的套接字。
    // len 指定 addr 指向的地址结构的大小（以字节为单位）。
    // 传统上，此操作称为“为套接字分配名称”。
    if (bind(listen_fd, (struct sockaddr *) &listen_address, sizeof(listen_address)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
    }
    // 准备接受套接字 fd 上的连接。
    // n 个连接请求将在进一步请求被拒绝之前排队。
    // 成功返回 0，错误返回 -1。
    if (listen(listen_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
    }

    if ((connect_fd = accept(listen_fd, (struct sockaddr *) nullptr, nullptr)) == -1) {
        printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
    }
    return connect_fd;
}
