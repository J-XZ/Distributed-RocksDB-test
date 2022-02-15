//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___NET_FILE_H
#define UNTITLED4___NET_FILE_H

#include "string"
#include "const_values/const_values.h"

using std::string;

#include "iostream"

using std::cout;
using std::endl;

#include "sys/socket.h"
#include "ip_location.h"

enum CONNECT_KIND {
    TO, FROM
};

class net_file {
public:
    net_file();

    int connect_fd;

    explicit net_file(int connect_fd);

    /// 创建一个socket连接描述符
    /// \param kind 这个连接是通过监听本机端口建立（from)还是通过向远端节点发起连接建立（to)
    /// \param port 端口号
    /// \param ip_address ip地址，如果是监听本机端口，则不需要这个参数
    net_file(CONNECT_KIND kind, int port, const string &ip_address = "");

    /// 向这个socket文件写入缓冲区inf中的len个字符
    /// \param inf 指向缓冲区起始位置的指针
    /// \param len 写入的字符量
    void send_to(char *inf, int len) const;

    /// 从这个socket读取len长度的字符，存储在缓冲区buff中
    /// \param buff 指向缓冲区起始位置的指针
    /// \param len 需要读取的字符数
    /// \return 成功读取的字符数
    int receive_from(char *buff, int len = MAXLINE) const;
};

#endif //UNTITLED4___NET_FILE_H
