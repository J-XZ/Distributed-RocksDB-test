//
// Created by jxz on 2022/2/14.
//

#include "handle_buff.h"

char *get_string_from_buff(const char *buff, string &ans) {
    int len;
    char tmp[MAXLINE];
    char *new_buff = (char *) buff;
    memcpy(&len, buff, sizeof(int));
    new_buff += sizeof(int);
    memcpy(tmp, new_buff, len);
    new_buff += len;
    ans = std::string(tmp);
    return new_buff;
}

char *insert_str_to_buff(char *start_address, const string &str, int &total_len) {
    int len = (int) str.length() + 1;
    char *ret = start_address;
    memcpy(ret, &len, sizeof(len));
    memcpy(ret += sizeof(len), str.c_str(), len);
    total_len += (int) len + (int) sizeof(len);
    return ret + len;
}

char *insert_int_to_buff(char *start_address, int a, int &total_len) {
    memcpy(start_address, &a, sizeof(int));
    total_len += sizeof(int);
    return start_address + sizeof(int);
}

const char *get_int_from_buff(const char *buff, int &ans) {
    int tmp;
    memcpy(&tmp, buff, sizeof(int));
    ans = tmp;
    return buff + sizeof(int);
}

const char *insert_str_s_to_buff(char *start_address, std::vector<string> str_s, int &total_size) {
    int total_size_t = 0;
    for (auto i: str_s) {
        start_address = insert_str_to_buff(start_address, i, total_size_t);
    }
    total_size += total_size_t;
    return start_address;
}
