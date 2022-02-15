//
// Created by jxz on 2022/2/14.
//

#include "control_meta.h"


Control_meta::Control_meta(const rocksdb::Slice slice) {
    const char *buff = slice.data();
    buff = get_string_from_buff(buff, min_str);
    buff = get_string_from_buff(buff, max_str);
    get_int_from_buff(buff, virtual_fd);
}

rocksdb::Slice Control_meta::to_slice(char *buff) const {
    char *tmp = buff;
    int total_len = 0;
    tmp = insert_str_to_buff(tmp, min_str, total_len);
    tmp = insert_str_to_buff(tmp, max_str, total_len);
    insert_int_to_buff(tmp, virtual_fd, total_len);
    return {buff, (size_t) total_len};
}

Control_meta::Control_meta(string min_str, string max_str, int virtual_fd) :
        min_str(std::move(min_str)), max_str(std::move(max_str)), virtual_fd(virtual_fd) {}

void Control_meta::show() const {
    cout << "min_str = " << min_str << " max_str = " << max_str << " virtual_fd = " << virtual_fd << endl;
}