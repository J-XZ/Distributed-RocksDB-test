//
// Created by jxz on 2022/2/14.
//

#ifndef UNTITLED4___HANDLE_BUFF_H
#define UNTITLED4___HANDLE_BUFF_H


#include <string>
#include "const_values/const_values.h"
#include "rocksdb/db.h"
#include "handle_buff.h"

using std::string;

/**
 * 字符串存储在缓冲区中的方式是：
 * 在缓冲区开始位置的4个字节存储一个int类型的值，表示字符串的长度len
 * 紧接着上面的位置，存储len字节的数据，即需要存储的字符串
 * 每次发送数据按照上述的方式存储字符串，一个缓存区可以包含多个字符串
 * */

/// 从缓冲区获取一个字符串
/// \param buff 缓冲区起始位置
/// \param ans 读取到的字符串
/// \return 指向缓冲区中下一个对象的指针
char *get_string_from_buff(const char *buff, std::string &ans);

/// 将一个字符串对象插入缓冲区
/// \param start_address 待插入数据的缓冲区的起始位置
/// \param str 需要插入缓冲区的字符串
/// \param total_len 缓冲区总长度，插入字符串之后这个变量增加字符串的长度（长度包含字符串末尾的\0）
/// \return 指向缓冲区下一个插入位置的指针
char *insert_str_to_buff(char *start_address, const std::string &str, int &total_len);

/// 将一个int类型变量插入缓冲区
/// \param start_address 指向缓冲区起始位置的指针
/// \param a 待插入的int对象
/// \param total_len 缓冲区总长度，插入int后需要增加4字节
/// \return 指向缓冲区下一个插入位置的指针
char *insert_int_to_buff(char *start_address, int a, int &total_len);

/// 从缓冲区读取一个int类型的变量
/// \param buff 指向缓冲区起始位置的指针
/// \param ans 获取的int对象
/// \return 指向缓冲区中下一个对象的指针
const char *get_int_from_buff(const char *buff, int &ans);

const char *insert_str_s_to_buff(char *start_address, std::vector<string> str_s, int &total_size);

#endif //UNTITLED4___HANDLE_BUFF_H
