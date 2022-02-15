//
// Created by jxz on 2022/2/14.
//

#include "store_log.h"
LOG::LOG(LOG_KIND kind, string key, string value) : kind(kind), key(std::move(key)), value(std::move(value)) {

}

rocksdb::Slice LOG::to_slice(char *const buff) {
    int total_len = 0;
    char *tmp = buff;
    tmp = insert_int_to_buff(tmp, (int) kind, total_len);
    tmp = insert_str_to_buff(tmp, key, total_len);
    tmp = insert_str_to_buff(tmp, value, total_len);
    return {buff, (size_t) total_len};
}

LOG::LOG(rocksdb::Slice &slice) {
    const char *tmp = (char *) slice.data();
    int a;
    tmp = get_int_from_buff(tmp, a);
    kind = (LOG_KIND) a;
    tmp = get_string_from_buff(tmp, key);
    tmp = get_string_from_buff(tmp, value);
}

LOG_KIND LOG::get_kind() {
    return kind;
}

string LOG::get_value() {
    return value;
}

string LOG::get_key() {
    return key;
}

LOG::LOG(const char *buff) {
    const char *tmp = (const char *) buff;
    int log_kind;
    tmp = get_int_from_buff(tmp, log_kind);
    kind = (LOG_KIND) log_kind;
    tmp = get_string_from_buff(tmp, key);
    get_string_from_buff(tmp, value);
}

void Store_Log::write_log(LOG_KIND kind, string &key, const string &value) {
    auto slice = LOG{kind, key, value}.to_slice(buff);
    db_handle.put(slice, "");
}

Store_Log::Store_Log(string &location) {
    db_handle = DB_Handle{location};
    buff = (char *) malloc(MAXLINE);
}

LOG Store_Log::get_log() {
    auto read_options = db_handle.get_read_options();
    auto db = db_handle.get_db();
    auto it = db->NewIterator(read_options);
    it->SeekToFirst();
    if (it->Valid()) {
        auto s = it->key();
        return LOG(s);
    }
    delete it;
    return LOG(ERROR);
}

void Store_Log::clear_log() {
    auto read_options = db_handle.get_read_options();
    auto db = db_handle.get_db();
    auto it = db->NewIterator(read_options);
    it->SeekToFirst();
    if (it->Valid()) {
        auto s = it->key();
        db_handle.delete_1(s);
    }
    delete it;
}

Store_Log::Store_Log() {
    db_handle = DB_Handle();
    buff = nullptr;
}
