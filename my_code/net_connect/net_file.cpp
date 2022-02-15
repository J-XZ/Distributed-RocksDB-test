//
// Created by jxz on 2022/2/14.
//

#include "net_file.h"

net_file::net_file(int connect_fd) : connect_fd(connect_fd) {}

void net_file::send_to(char *inf, int len) const {
    send(connect_fd, inf, len, 0);
}

net_file::net_file(CONNECT_KIND kind, int port, const string& ip_address) {
    switch (kind) {
        case TO:
            connect_fd = Ip_location{ip_address, port}.connect_to();
            break;
        case FROM:
            connect_fd = Ip_location::receive_from(port);
            cout<<"连接成功"<<endl;
            break;
    }
}

int net_file::receive_from(char *buff, int len) const {
    return (int) recv(connect_fd, buff, len, 0);
}

net_file::net_file() {
    connect_fd = -1;
}
