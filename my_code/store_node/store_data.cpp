//
// Created by jxz on 2022/2/14.
//

#include "store_data.h"

Data::Data(string key, string value) : key(std::move(key)), value(std::move(value)) {}

Data::Data(char *key, char *value) {
    this->key = string{key};
    this->value = string{value};
}

store_data::store_data(string &location) {
    db_handle = DB_Handle{location};
    buff = (char *) malloc(MAXLINE);
    size = db_handle.get_size();
}

string store_data::get(string key) {
    return db_handle.get(key);
}

void store_data::delete_1(string key) {
    string value = db_handle.get(key);
    if (value.length()) {
        size -= (int) value.length() + 1;
        size -= (int) key.length() + 1;
    }
    db_handle.delete_1(key);
}

void store_data::put(const Data &data) {
    string key = data.key;
    string value = db_handle.get(key);
    if (value.length()) {
        size -= (int) value.length() + 1;
        size -= (int) key.length() + 1;
    }
    db_handle.put(data.key, data.value);
    size += (int) data.key.length() + (int) data.value.length() + 2;
}

int store_data::get_size() const {
    return size;
}

store_data::store_data() {
    buff = nullptr;
    size = 0;
    db_handle = DB_Handle();
}

DB_Handle *store_data::get_db() {
    return &db_handle;
}

void store_data::get_max_key(string &max_key, string &value) {
    auto it = db_handle.get_db()->NewIterator(db_handle.get_read_options());
    it->SeekToLast();
    if (it->Valid()) {
        max_key = it->key().ToString();
        value = it->value().ToString();
    }
    delete it;
}

void store_data::get_min_key(string &min_key, string &value) {
    auto it = db_handle.get_db()->NewIterator(db_handle.get_read_options());
    it->SeekToFirst();
    if (it->Valid()) {
        min_key = it->key().ToString();
        value = it->value().ToString();
    }
    delete it;
}
