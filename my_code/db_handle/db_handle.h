//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___DB_HANDLE_H
#define UNTITLED4___DB_HANDLE_H

#include "rocksdb/db.h"
#include "string"
#include "iostream"

using std::cout;
using std::string;
using std::to_string;

class DB_Handle {
private:
    rocksdb::DB *db{};
    rocksdb::WriteOptions write_options;
    rocksdb::Options options;
    rocksdb::ReadOptions readOptions;
public:
    DB_Handle();;

    explicit DB_Handle(DB_Handle const &) = delete;

    explicit DB_Handle(string &db_location);

    void put(char *key_buf, int key_len, char *value_buf, int value_len);

    void put(const string &key, const string &value);

    void put(string &key, rocksdb::Slice &value);

    void put(char *key_buf, int key_len, rocksdb::Slice &value);

    void put(const string &key, rocksdb::Slice &value);

    void put(rocksdb::Slice &key, const string &value = "");

    void put(unsigned int num, rocksdb::Slice &value);

    /// 在数据库中检索键等与目标键的条目
    /// \param key_buf 指向目标键的指针
    /// \param key_len 目标键的长度
    /// \param iterator 返回迭代器
    /// \return 存在目标键则返回true，否则false
    bool get(char *key_buf, int key_len, rocksdb::Iterator **iterator);

    bool get(string &key, rocksdb::Iterator **iterator);

    string get(string &key);

    void delete_1(rocksdb::Slice slice);

    static bool get_smaller_or_equal_but_as_large_as_possible(const string &key, rocksdb::Iterator **iterator);

    rocksdb::DB *get_db();

    rocksdb::ReadOptions get_read_options();

    int get_num();

    int get_size();

    bool store_less_or_equal_one();
};


#endif //UNTITLED4___DB_HANDLE_H
