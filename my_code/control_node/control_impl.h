//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___CONTROL_IMPL_H
#define UNTITLED4___CONTROL_IMPL_H

#include <cmath>
#include "control_meta.h"
#include "map"
#include "vector"
#include "net_connect/ip_location.h"
#include "net_connect/net_file.h"
#include "iostream"
#include "db_handle/db_handle.h"
#include "control_meta.h"
#include "store_node/store_log.h"

using std::cin;
using std::cout;
using std::endl;
using std::map;

class control_impl {
private:
    int store_node_count;
    DB_Handle meta_db;
    map<int, int> virtual_fd_2_real_fd;
    char *send_buff;
    char *receive_buff;

    int get_size(int virtual_fd);

    bool get_store_less_or_equal_one(int virtual_fd);

    bool not_too_much(int virtual_fd, int total_size, double perfect);

    void get_max_store(int virtual_fd, string &max_key, string &max_value);

    void get_min_store(int virtual_fd, string &min_key, string &value);

    void show_meta();

    void re_balance();

public:
    net_file *virtual_fd_2_net_file;

    explicit control_impl(string &control_meta_path, std::vector<Ip_location> store_nodes);

    void run();
};


#endif //UNTITLED4___CONTROL_IMPL_H
