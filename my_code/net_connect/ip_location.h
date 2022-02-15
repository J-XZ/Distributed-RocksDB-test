//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___IP_LOCATION_H
#define UNTITLED4___IP_LOCATION_H

#include "string"
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

using std::string;


class Ip_location {
public:
    /// 初始化ip地址类
    /// \param ip_address ipv4地址
    /// \param port 端口号
    Ip_location(const string &ip_address, int port);

    /// 创建一个本机socket，并连接到sockaddr_in地址的socket
    /// \return socket描述符
    int connect_to();

    static int receive_from(int port);

    Ip_location() = default;

private:
    string ip_address;
    int port{};
    struct sockaddr_in store_node_address{};
};


#endif //UNTITLED4___IP_LOCATION_H
