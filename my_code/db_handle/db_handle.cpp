//
// Created by jxz on 2022/2/14.
//

#include "db_handle.h"


void DB_Handle::put(const string &key, rocksdb::Slice &value) {
    db->Put(write_options, key, value);
}

void DB_Handle::put(char *key_buf, int key_len, rocksdb::Slice &value) {
    db->Put(write_options, {key_buf, (size_t) key_len}, value);
}

void DB_Handle::put(const string &key, const string &value) {
    db->Put(write_options, key, value);
}

void DB_Handle::put(char *key_buf, int key_len, char *value_buf, int value_len) {
    db->Put(write_options, {key_buf, (size_t) key_len}, {value_buf, (size_t) value_len});
}

DB_Handle::DB_Handle(string &db_location) {
    readOptions = rocksdb::ReadOptions();
    write_options = rocksdb::WriteOptions();
    options = rocksdb::Options();
    options.create_if_missing = true;
    auto status = rocksdb::DB::Open(options, db_location, &db);
    assert(status.ok());
}

bool DB_Handle::get(char *key_buf, int key_len, rocksdb::Iterator **iterator) {
    *iterator = db->NewIterator(readOptions);
    (*iterator)->Seek({key_buf, (size_t) key_len});
    if ((*iterator)->Valid() && !memcpy((void *) ((*iterator)->key().data()), key_buf, key_len)) {
        return true;
    }
    return false;
}

bool DB_Handle::get(string &key, rocksdb::Iterator **iterator) {
    return get((char *) key.c_str(), (int) key.length() + 1, iterator);
}

void DB_Handle::put(rocksdb::Slice &key, const string &value) {
    db->Put(write_options, key, value);
}

void DB_Handle::delete_1(rocksdb::Slice slice) {
    db->Delete(write_options, slice);
}

rocksdb::DB *DB_Handle::get_db() {
    return db;
}

rocksdb::ReadOptions DB_Handle::get_read_options() {
    return readOptions;
}

int DB_Handle::get_num() {
    int ret = 0;
    auto it = db->NewIterator(readOptions);
    it->SeekToFirst();
    while (it->Valid()) {
        ret++;
        it->Next();
    }
    delete it;
    return ret;
}

void DB_Handle::put(unsigned int num, rocksdb::Slice &value) {
    string n = to_string(num);
    db->Put(write_options, n, value);
}

int DB_Handle::get_size() {
    int ret = 0;
    auto it = db->NewIterator(readOptions);
    it->SeekToFirst();
    while (it->Valid()) {
        ret += (int) it->value().size() + 1;
        ret += (int) it->key().size() + 1;
        it->Next();
    }
    return ret;
}

string DB_Handle::get(string &key) {
    string ret;
    db->Get(readOptions, key, &ret);
    return ret;
}

void DB_Handle::put(string &key, rocksdb::Slice &value) {
    db->Put(write_options, key, value);
}

bool DB_Handle::get_smaller_or_equal_but_as_large_as_possible(const string &key, rocksdb::Iterator **iterator) {
    (*iterator)->Seek({key.c_str(), key.length() + 1}); // 找到第一个大于等于目标元素的元素
    if (!(*iterator)->Valid()) { // 每个元素都比目标元素小
        (*iterator)->SeekToLast(); // 返回尽可能大的元素
        return true;
    }
    if ((*iterator)->key() == key) { // 找到目标元素
        return true;
    }
    // 找到大于目标元素的元素
    (*iterator)->Prev();
    if (!(*iterator)->Valid()) { // 每个元素都比目标元素大
        (*iterator)->SeekToFirst();
        return false;
    } else { // 找到第一个小于等于目标元素的元素
        return true;
    }
}

bool DB_Handle::store_less_or_equal_one() {
    int ret = 0;
    auto it = db->NewIterator(readOptions);
    it->SeekToFirst();
    while (it->Valid()) {
        ret++;
        it->Next();
        if (ret >= 2) {
            delete it;
            return false;
        }
    }
    delete it;
    return true;
}

DB_Handle::DB_Handle() {
    write_options = rocksdb::WriteOptions();
    options = rocksdb::Options();
    readOptions = rocksdb::ReadOptions();
}


