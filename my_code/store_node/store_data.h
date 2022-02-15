//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___STORE_DATA_H
#define UNTITLED4___STORE_DATA_H

#include "string"

using std::string;

#include "db_handle/db_handle.h"
#include "const_values/const_values.h"
#include "iostream"

using std::cout;
using std::endl;

class Data {
public:
    string key, value;

    Data(string key, string value);

    Data(char *key, char *value);
};

class store_data {
private:
    DB_Handle db_handle;
    char *buff;
    int size;
public:
    DB_Handle *get_db();

    store_data();

    int get_size() const;

    explicit store_data(string &location);

    store_data(store_data const &) = delete;

    /// 存储一个键值对，并修改总容量记录
    /// \param data 需要存储的键值对
    void put(const Data &data);

    /// 获取键对应的值
    /// \param key 需要查询的键
    /// \return 查询到的值
    string get(string key);

    /// 删除一个键对应的值，并修改总容量记录
    /// \param key 需要删除的键
    void delete_1(string key);

    void get_max_key(string &max_key, string &value);

    void get_min_key(string &min_key, string &value);
};


#endif //UNTITLED4___STORE_DATA_H
