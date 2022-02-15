//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___STORE_LOG_H
#define UNTITLED4___STORE_LOG_H

#include "string"

using std::string;

#include "db_handle/db_handle.h"
#include "const_values/const_values.h"
#include "iostream"

using std::cout;
using std::endl;

#include "handle_buff/handle_buff.h"

enum LOG_KIND {
    PUT, DELETE, ERROR
};

class LOG {
private:
    LOG_KIND kind;
    string key, value;
public:
    explicit LOG(LOG_KIND kind, string key = "", string value = "");

    explicit LOG(rocksdb::Slice &slice);

    explicit LOG(const char *buff);

    rocksdb::Slice to_slice(char *buff);

    LOG_KIND get_kind();

    string get_key();

    string get_value();
};

class Store_Log {
private:
    DB_Handle db_handle;
    char *buff;
public:
    Store_Log();

    explicit Store_Log(string &location);

    Store_Log(Store_Log const &) = delete;

    /// 写日志
    /// \param kind 日志类型，是插入元素还是删除元素，只有修改操作需要先写日志
    /// \param key 修改的目标键
    /// \param value 修改的值
    void write_log(LOG_KIND kind, string &key, const string &value = "");

    /// 获取当前存储的日志
    /// \return 当前存储的日志
    LOG get_log();

    /// 清空日志
    void clear_log();
};


#endif //UNTITLED4___STORE_LOG_H
