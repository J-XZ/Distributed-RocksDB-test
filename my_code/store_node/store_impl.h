//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___STORE_IMPL_H
#define UNTITLED4___STORE_IMPL_H

#include <utility>

#include "store_data.h"
#include "store_log.h"
#include "net_connect/net_file.h"

class store_impl {
private:
    net_file connection;
    store_data storeData;
    Store_Log storeLog;
    char *receive_buff = nullptr;
    char *send_buff = nullptr;


    void put(string key, string value);

    void delete_1(string key);

    void log_put(string key, const string &value);

    void log_delete(string key);

    void commit_log();

    void delete_log();

    string get(string key);

    void log_log(LOG log, char *buff, int &send_len);

public:
    bool store_less_or_equal_one();

    store_impl(string data_path, string log_path, int port);

    void run();
};


#endif //UNTITLED4___STORE_IMPL_H
