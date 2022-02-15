//
// Created by jxz on 2022/2/14.
//

#include "store_impl.h"

store_impl::store_impl(string data_path, string log_path, int port) {
    storeData = store_data{data_path};
    storeLog = Store_Log(log_path);
    connection = net_file{FROM, port};
    receive_buff = (char *) malloc(MAXLINE);
    send_buff = (char *) malloc(MAXLINE);
}

void store_impl::put(string key, string value) {
    storeData.put({std::move(key), std::move(value)});
}

void store_impl::log_put(string key, const string &value) {
    storeLog.write_log(PUT, key, value);
}

void store_impl::log_delete(string key) {
    storeLog.write_log(DELETE, key);
}

void store_impl::delete_log() {
    storeLog.clear_log();
}

void store_impl::delete_1(string key) {
    storeData.delete_1(std::move(key));
}

void store_impl::commit_log() {
    LOG log = storeLog.get_log();
    switch (log.get_kind()) {
        case PUT:
            put(log.get_key(), log.get_value());
            break;
        case DELETE:
            delete_1(log.get_key());
            break;
        case ERROR:
            cout << "log_error" << endl;
            break;
    }
    delete_log();
}

string store_impl::get(string key) {
    return storeData.get(std::move(key));
}

void store_impl::log_log(LOG log, char *buff, int &send_len) {
    string new_min, new_max;
    auto db = storeData.get_db();
    auto it = db->get_db()->NewIterator(db->get_read_options());
    switch (log.get_kind()) {
        case PUT:
            log_put(log.get_key(), log.get_value());

            it->SeekToFirst();
            if (it->Valid()) {
                new_min = it->key().ToString();
            } else { // 当前为空
                new_min = "";
            }
            it->SeekToLast();
            if (it->Valid()) {
                new_max = it->key().ToString();
            } else {
                new_max = "";
            }
            if (new_min.empty()) {
                new_min = log.get_key();
                new_max = log.get_key();
            }
            buff = insert_str_to_buff(buff, min(new_min, log.get_key()), send_len);
            insert_str_to_buff(buff, max(log.get_key(), new_max), send_len);

            break;
        case DELETE:
            log_delete(log.get_key());

            it->SeekToFirst();
            if (it->Valid()) {
                new_min = it->key().ToString();
            } else { // 当前为空
                new_min = "";
            }
            it->SeekToLast();
            if (it->Valid()) {
                new_max = it->key().ToString();
            } else {
                new_max = "";
            }
            if (log.get_key() == new_min) {
                it->SeekToFirst();
                it->Next();
                if (!it->Valid()) {
                    new_min = "";
                } else {
                    new_min = it->key().ToString();
                }
            }
            if (log.get_key() == new_max) {
                it->SeekToLast();
                it->Prev();
                if (!it->Valid()) {
                    new_max = "";
                } else {
                    new_max = it->key().ToString();
                }
            }
            buff = insert_str_to_buff(buff, new_min, send_len);
            insert_str_to_buff(buff, new_max, send_len);

            break;
        case ERROR:
            cout << "log_error" << endl;
            break;
    }
    delete it;
}

void store_impl::run() {
    while (true) {
        cout << connection.receive_from(receive_buff) << endl;
        char *tmp = receive_buff;
        string command;
        tmp = get_string_from_buff(tmp, command);
        if (command == get_QUIT()) {
            break;
        } else if (command == get_GET()) {
            string key;
            get_string_from_buff(tmp, key);
            auto value = get(key);
            int total_size = 0;
            insert_str_to_buff(send_buff, value, total_size);
            connection.send_to(send_buff, total_size);
        } else if (command == get_LOG()) {
            char *send = send_buff;
            int send_len = 0;
            auto log = LOG{tmp};
            send = insert_str_to_buff(send, get_OK(), send_len);
            log_log(log, send, send_len);
            connection.send_to(send_buff, send_len);
        } else if (command == get_COMMIT_LOG()) {
            int send_len = 0;
            char *send = send_buff;
            insert_str_to_buff(send, get_OK(), send_len);
            commit_log();
            connection.send_to(send_buff, send_len);
        } else if (command == get_TEST()) {
            string rec;
            get_string_from_buff(tmp, rec);
            cout << "receive " << rec << endl;
        } else if (command == get_SIZE()) {
            char *send = send_buff;
            int total_len = 0;
            insert_int_to_buff(send, storeData.get_size(), total_len);
            connection.send_to(send_buff, total_len);
        } else if (command == get_LESS_OR_EQUAL_ONE()) {
            char *send = send_buff;
            int total_len = 0;
            insert_int_to_buff(send, (int) store_less_or_equal_one(), total_len);
            connection.send_to(send_buff, total_len);
        } else if (command == get_MAX_KEY()) {
            int total_len = 0;
            char *send = send_buff;
            string max_key, value;
            storeData.get_max_key(max_key, value);
            send = insert_str_to_buff(send, max_key, total_len);
            insert_str_to_buff(send, value, total_len);
            connection.send_to(send_buff, total_len);
        } else if (command == get_MIN_KEY()) {
            int total_len = 0;
            char *send = send_buff;
            string max_key, value;
            storeData.get_min_key(max_key, value);
            send = insert_str_to_buff(send, max_key, total_len);
            insert_str_to_buff(send, value, total_len);
            connection.send_to(send_buff, total_len);
        } else if (command == get_DEL_LOG()) {
            char *send = send_buff;
            int send_len = 0;
            delete_log();
            insert_str_to_buff(send, get_OK(), send_len);
            connection.send_to(send_buff, send_len);
        }
    }
}

bool store_impl::store_less_or_equal_one() {
    return storeData.get_db()->store_less_or_equal_one();
}
