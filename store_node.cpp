#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cerrno>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include <iostream>
#include "cstring"
#include "defs.h"
#include "rocksdb/db.h"

using namespace std;

int main(int argc, char **argv) {
    rocksdb::DB *db;
    rocksdb::Options options{};
    options.create_if_missing = true;
    auto s = rocksdb::DB::Open(options, "/tmp/testdb", &db);
    cout << s.ok() << endl;
    string str;
    db->Put(rocksdb::WriteOptions(), "test", "ok");
    cout << "put ok";
    db->Get(rocksdb::ReadOptions(), "test", &str);
    cout << "get " << str << endl;

    int listen_fd, connection_fd;
    struct sockaddr_in control_address{};
    char *buff = (char *) malloc(MAXLINE);
    int n;

    // 使用协议 protocol 在域 domain 中创建 type 类型的新套接字。
    // 如果 protocol 为零，则自动选择一个。
    // 返回新套接字的文件描述符，或 -1 表示错误。
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    control_address.sin_family = AF_INET;
    control_address.sin_addr.s_addr = htonl(INADDR_ANY);
    control_address.sin_port = htons(STORE_LISTEN_CONTROL);

    // 当使用 socket 函数创建套接字时，它存在于名称空间（地址族）中，但没有分配给它的地址。
    // bind() 将 addr 指定的地址分配给文件描述符 fd 引用的套接字。
    // len 指定 addr 指向的地址结构的大小（以字节为单位）。
    // 传统上，此操作称为“为套接字分配名称”。
    if (bind(listen_fd, (struct sockaddr *) &control_address, sizeof(control_address)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    // 准备接受套接字 fd 上的连接。
    // n 个连接请求将在进一步的请求被拒绝之前排队。
    // 成功返回 0，错误返回 -1。
    if (listen(listen_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    printf("======waiting for client's request======\n");
    while (1) {
        // 在一个死循环中不断监听6666端口的连接
        if ((connection_fd = accept(listen_fd, (struct sockaddr *) NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)", strerror(errno), errno);
            continue;
        }
        n = recv(connection_fd, buff, MAXLINE, 0);
        buff[n] = '\0';
        printf("recv msg from client: %s\n", buff);
        close(connection_fd);
    }
    close(listen_fd);
    return 0;
}