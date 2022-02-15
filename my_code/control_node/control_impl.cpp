//
// Created by jxz on 2022/2/14.
//

#include "control_impl.h"

control_impl::control_impl(string &control_meta_path, std::vector<Ip_location> store_nodes) {
    meta_db = DB_Handle{control_meta_path};
    int size = meta_db.get_num();
    send_buff = (char *) malloc(MAXLINE);
    receive_buff = (char *) malloc(MAXLINE);
    virtual_fd_2_net_file = (net_file *) malloc(store_nodes.size());
    store_node_count = 0;
    for (int i = 0; i < store_nodes.size(); i++) {
        store_node_count++;
        virtual_fd_2_real_fd[i] = store_nodes[i].connect_to();
        int real_fd = virtual_fd_2_real_fd[i];
        auto netFile = net_file{real_fd};
        virtual_fd_2_net_file[i] = netFile;
        if (!size) {
            auto control_meta = Control_meta{to_string(i + 1), to_string(i + 1), i};
            string min_str = control_meta.min_str;
            auto s = control_meta.to_slice(send_buff);
            meta_db.put(min_str, s);
        }
    }
    send_buff = (char *) malloc(MAXLINE);
    receive_buff = (char *) malloc(MAXLINE);
}

int control_impl::get_size(int virtual_fd) {
    int ret = 0;
    char *send = send_buff;
    insert_str_to_buff(send, get_SIZE(), ret);
    virtual_fd_2_net_file[virtual_fd].send_to(send_buff, ret);
    virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
    get_int_from_buff(receive_buff, ret);
    return ret;
}

bool control_impl::get_store_less_or_equal_one(int virtual_fd) {
    int ret = 0;
    char *send = send_buff;
    insert_str_to_buff(send, get_LESS_OR_EQUAL_ONE(), ret);
    virtual_fd_2_net_file[virtual_fd].send_to(send_buff, ret);
    virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
    get_int_from_buff(receive_buff, ret);
    return ret;
}

bool control_impl::not_too_much(int virtual_fd, int total_size, double perfect) {
    double s = get_size(virtual_fd);
    return (s / (total_size + 0.000001)) <= perfect + 0.01||
           get_store_less_or_equal_one(virtual_fd);
}

void control_impl::get_max_store(int virtual_fd, string &max_key, string &max_value) {
    int total_size = 0;
    char *send = send_buff;
    char *rec = receive_buff;
    insert_str_to_buff(send, get_MAX_KEY(), total_size);
    virtual_fd_2_net_file[virtual_fd].send_to(send_buff, total_size);
    virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
    rec = get_string_from_buff(rec, max_key);
    get_string_from_buff(rec, max_value);
}

void control_impl::get_min_store(int virtual_fd, string &min_key, string &value) {
    int total_size = 0;
    char *send = send_buff;
    char *rec = receive_buff;
    insert_str_to_buff(send, get_MIN_KEY(), total_size);
    virtual_fd_2_net_file[virtual_fd].send_to(send_buff, total_size);
    virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
    rec = get_string_from_buff(rec, min_key);
    get_string_from_buff(rec, value);
}

void control_impl::show_meta() {
    auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
    it->SeekToFirst();
    while (it->Valid()) {
        cout << "size == " << get_size(Control_meta{it->value()}.virtual_fd) << endl;
        Control_meta{it->value()}.show();
        it->Next();
    }
    delete it;
}

void control_impl::re_balance() {
    int total_size_x = 0, total_size;
    for (int i = 0; i < store_node_count; i++) {
        total_size_x += get_size(i);
    }
    double perfect = (double) 1 / store_node_count;
    for (int i = 0; i < store_node_count - 1; i++) {
        while (!not_too_much(i, total_size_x, perfect)) {
            string max_key, value;
            get_max_store(i, max_key, value);
            char *tmp = send_buff;
            total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            auto s = LOG{DELETE, max_key}.to_slice(tmp);

            virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[i].receive_from(receive_buff);
            string ans;
            char *rec = receive_buff;
            string new_max_1, new_min_1;
            rec = get_string_from_buff(rec, ans);
            rec = get_string_from_buff(rec, new_min_1);
            get_string_from_buff(rec, new_max_1);

            bool can_commit = true;
            if (ans != "ok") {
                can_commit = false;
                cout << "error" << endl;
            }

            tmp = send_buff;
            total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            s = LOG{PUT, max_key, value}.to_slice(tmp);

            virtual_fd_2_net_file[i + 1].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[i + 1].receive_from(receive_buff);
            rec = receive_buff;
            string new_max_2, new_min_2;
            rec = get_string_from_buff(rec, ans);
            rec = get_string_from_buff(rec, new_min_2);
            get_string_from_buff(rec, new_max_2);

            if (ans != "ok") {
                can_commit = false;
                cout << "error" << endl;
            }

            // 如果任意一个节点没有成功写入，则所有节点都回退
            if (!can_commit) {
                tmp = send_buff;
                total_size = 0;
                insert_str_to_buff(tmp, get_DEL_LOG(), total_size);
                virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[i].receive_from(receive_buff);
                virtual_fd_2_net_file[i + 1].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[i + 1].receive_from(receive_buff);
            }

            // 到此涉及数据迁移的两个节点各自成功写入日志

            // 修改控制节点元数据
            auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
            DB_Handle::get_smaller_or_equal_but_as_large_as_possible(max_key, &it);

            Control_meta controlMeta1{it->value()};
            if (!new_min_1.empty())
                controlMeta1.min_str = new_min_1;
            if (!new_max_1.empty())
                controlMeta1.max_str = new_max_1;

            meta_db.delete_1(it->key());

            string new_key = controlMeta1.min_str;
            auto ss = controlMeta1.to_slice(receive_buff);
            meta_db.put(new_key, ss);

            it->Next();
            controlMeta1 = Control_meta{it->value()};
            if (!new_min_2.empty())
                controlMeta1.min_str = new_min_2;
            if (!new_max_2.empty())
                controlMeta1.max_str = new_max_2;

            meta_db.delete_1(it->key());

            new_key = controlMeta1.min_str;
            ss = controlMeta1.to_slice(receive_buff);
            meta_db.put(new_key, ss);

            delete it;

            // 两个节点各自提交
            tmp = send_buff;
            total_size = 0;
            insert_str_to_buff(tmp, get_COMMIT_LOG(), total_size);
            virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size);
            virtual_fd_2_net_file[i].receive_from(receive_buff);
            get_string_from_buff(receive_buff, ans);
            if (ans != "ok") {
                cout << "error" << endl;
                return;
            }
            virtual_fd_2_net_file[i + 1].send_to(send_buff, (int) total_size);
            virtual_fd_2_net_file[i + 1].receive_from(receive_buff);
            get_string_from_buff(receive_buff, ans);
            if (ans != "ok") {
                cout << "error" << endl;
                return;
            }
        }
    }
    for (int i = store_node_count - 1; i > 0; i--) {
        while (!not_too_much(i, total_size_x, perfect)) {
            string min_key, value;
            get_min_store(i, min_key, value);

            char *tmp = send_buff;
            total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            auto s = LOG{DELETE, min_key}.to_slice(tmp);

            virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[i].receive_from(receive_buff);
            string ans;
            char *rec = receive_buff;
            string new_max_1, new_min_1;
            rec = get_string_from_buff(rec, ans);
            rec = get_string_from_buff(rec, new_min_1);
            get_string_from_buff(rec, new_max_1);

            bool can_commit = true;
            if (ans != "ok") {
                can_commit = false;
                cout << "error" << endl;
            }

            tmp = send_buff;
            total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            s = LOG{PUT, min_key, value}.to_slice(tmp);

            virtual_fd_2_net_file[i - 1].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[i - 1].receive_from(receive_buff);
            rec = receive_buff;
            string new_max_2, new_min_2;
            rec = get_string_from_buff(rec, ans);
            rec = get_string_from_buff(rec, new_min_2);
            get_string_from_buff(rec, new_max_2);


            if (ans != "ok") {
                can_commit = false;
                cout << "error" << endl;
            }

            // 如果任意一个节点没有成功写入，则所有节点都回退
            if (!can_commit) {
                tmp = send_buff;
                total_size = 0;
                insert_str_to_buff(tmp, get_DEL_LOG(), total_size);
                virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[i].receive_from(receive_buff);
                virtual_fd_2_net_file[i - 1].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[i - 1].receive_from(receive_buff);
            }

            // 到此涉及数据迁移的两个节点各自成功写入日志

            // 修改控制节点元数据
            auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
            DB_Handle::get_smaller_or_equal_but_as_large_as_possible(min_key, &it);

            Control_meta controlMeta1{it->value()};
            if (!new_min_1.empty())
                controlMeta1.min_str = new_min_1;
            if (!new_max_1.empty())
                controlMeta1.max_str = new_max_1;

            meta_db.delete_1(it->key());

            string new_key = controlMeta1.min_str;
            auto ss = controlMeta1.to_slice(receive_buff);
            meta_db.put(new_key, ss);

            it->Prev();
            controlMeta1 = Control_meta{it->value()};
            if (!new_min_2.empty())
                controlMeta1.min_str = new_min_2;
            if (!new_max_2.empty())
                controlMeta1.max_str = new_max_2;

            meta_db.delete_1(it->key());

            new_key = controlMeta1.min_str;
            ss = controlMeta1.to_slice(receive_buff);
            meta_db.put(new_key, ss);

            delete it;

            // 两个节点各自提交
            tmp = send_buff;
            total_size = 0;
            insert_str_to_buff(tmp, get_COMMIT_LOG(), total_size);
            virtual_fd_2_net_file[i].send_to(send_buff, (int) total_size);
            virtual_fd_2_net_file[i].receive_from(receive_buff);
            get_string_from_buff(receive_buff, ans);
            if (ans != "ok") {
                cout << "error" << endl;
                return;
            }
            virtual_fd_2_net_file[i - 1].send_to(send_buff, (int) total_size);
            virtual_fd_2_net_file[i - 1].receive_from(receive_buff);
            get_string_from_buff(receive_buff, ans);
            if (ans != "ok") {
                cout << "error" << endl;
                return;
            }
        }
    }
    show_meta();

}

void control_impl::run() {
    while (true) {
        string command;
        cin >> command;
        if (command == "quit") {
            break;
        } else if (command == get_TEST()) {
            string test_string;
            cin >> test_string;
            char *tmp = send_buff;
            int total_size = 0;
            insert_str_s_to_buff(tmp, {{get_TEST(), test_string}}, total_size);
            virtual_fd_2_net_file[0].send_to(send_buff, total_size);
        } else if (command == get_PUT()) {
            string key, value;
            cin >> key >> value;
            cout << "putting " << key << " " << value << endl;
            char *tmp = send_buff;
            int total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            auto s = LOG{PUT, key, value}.to_slice(tmp);
            auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
            DB_Handle::get_smaller_or_equal_but_as_large_as_possible(key, &it);
            int virtual_fd = Control_meta{it->value()}.virtual_fd;
            virtual_fd_2_net_file[virtual_fd].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
            string ans;
            get_string_from_buff(receive_buff, ans);
            if (ans == "ok") {
                cout << "log ok" << endl;
                Control_meta controlMeta{it->value()};
                controlMeta.min_str = min(controlMeta.min_str, key);
                controlMeta.max_str = max(controlMeta.max_str, key);
                meta_db.delete_1(it->key());
                string new_key = controlMeta.min_str;
                auto ss = controlMeta.to_slice(receive_buff);
                meta_db.put(new_key, ss);
                controlMeta.show();
                tmp = send_buff;
                total_size = 0;
                insert_str_to_buff(tmp, get_COMMIT_LOG(), total_size);
                virtual_fd_2_net_file[virtual_fd].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
                get_string_from_buff(receive_buff, ans);
                if (ans == "ok") {
                    cout << "log commit" << endl;
                }
            }
            delete it;
            re_balance();
        } else if (command == get_GET()) {
            string key;
            cin >> key;
            auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
            DB_Handle::get_smaller_or_equal_but_as_large_as_possible(key, &it);
            Control_meta controlMeta{it->value()};
            auto netFile = virtual_fd_2_net_file[controlMeta.virtual_fd];
            char *tmp = send_buff;
            char *rec = receive_buff;
            int send_len = 0;
            tmp = insert_str_to_buff(tmp, get_GET(), send_len);
            insert_str_to_buff(tmp, key, send_len);
            netFile.send_to(send_buff, send_len);
            netFile.receive_from(receive_buff);
            string ans;
            get_string_from_buff(rec, ans);
            cout << "get " << ans << endl;
            delete it;
        } else if (command == get_DEL()) {
            string key;
            cin >> key;
            if (key.empty()) {
                continue;
            }
            cout << "deleting " << key << endl;
            char *tmp = send_buff;
            int total_size = 0;
            tmp = insert_str_to_buff(tmp, get_LOG(), total_size);
            auto s = LOG{DELETE, key}.to_slice(tmp);
            auto it = meta_db.get_db()->NewIterator(meta_db.get_read_options());
            DB_Handle::get_smaller_or_equal_but_as_large_as_possible(key, &it);
            int virtual_fd = Control_meta{it->value()}.virtual_fd;
            virtual_fd_2_net_file[virtual_fd].send_to(send_buff, (int) total_size + (int) s.size());
            virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
            string ans;
            char *rec = receive_buff;
            rec = get_string_from_buff(rec, ans);
            if (ans == "ok") {
                cout << "log ok" << endl;
                Control_meta controlMeta{it->value()};
                string new_min, new_max;
                rec = get_string_from_buff(rec, new_min);
                get_string_from_buff(rec, new_max);
                if (new_min.empty()) {
                    new_min = controlMeta.min_str;
                    new_max = controlMeta.max_str;
                }
                controlMeta.min_str = min(controlMeta.min_str, new_min);
                controlMeta.max_str = max(controlMeta.max_str, key);
                meta_db.delete_1(it->key());
                string new_key = controlMeta.min_str;
                auto ss = controlMeta.to_slice(receive_buff);
                controlMeta.show();
                meta_db.put(new_key, ss);
                tmp = send_buff;
                total_size = 0;
                insert_str_to_buff(tmp, get_COMMIT_LOG(), total_size);
                virtual_fd_2_net_file[virtual_fd].send_to(send_buff, (int) total_size);
                virtual_fd_2_net_file[virtual_fd].receive_from(receive_buff);
                get_string_from_buff(receive_buff, ans);
                if (ans == "ok") {
                    cout << "log commit" << endl;
                }
            }
            delete it;
            re_balance();
        }
    }
}
