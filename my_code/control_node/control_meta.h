//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___CONTROL_META_H
#define UNTITLED4___CONTROL_META_H


#include "rocksdb/slice.h"
#include "string"
#include "handle_buff/handle_buff.h"
#include "iostream"

using std::cout;
using std::endl;
using std::string;

class Control_meta {
public:
    string min_str, max_str;
    int virtual_fd{};

    void show() const;

    Control_meta(string min_str, string max_str, int virtual_fd);

    explicit Control_meta(rocksdb::Slice slice);

    rocksdb::Slice to_slice(char *buff) const;
};


#endif //UNTITLED4___CONTROL_META_H
