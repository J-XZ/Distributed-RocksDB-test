cmake_minimum_required(VERSION 3.10)
project(untitled4__)
include_directories(rocksdb_include
        my_code)

set(CMAKE_CXX_STANDARD 14)

add_library(my_lib
        my_code/net_connect/ip_location.cpp
        my_code/net_connect/ip_location.h
        my_code/const_values/const_values.cpp
        my_code/const_values/const_values.h
        my_code/control_node/control_meta.cpp
        my_code/control_node/control_meta.h
        my_code/handle_buff/handle_buff.cpp
        my_code/handle_buff/handle_buff.h
        my_code/db_handle/db_handle.cpp
        my_code/db_handle/db_handle.h
        my_code/store_node/store_data.cpp
        my_code/store_node/store_data.h
        my_code/store_node/store_log.cpp
        my_code/store_node/store_log.h
        my_code/store_node/store_impl.cpp
        my_code/store_node/store_impl.h
        my_code/net_connect/net_file.cpp
        my_code/net_connect/net_file.h
        my_code/control_node/control_impl.cpp
        my_code/control_node/control_impl.h)

add_executable(control_node
        my_code/main_cpp/control_node.cpp)

add_executable(store_node
        my_code/main_cpp/store_node.cpp)

target_link_libraries(control_node
        my_lib
        ${CMAKE_SOURCE_DIR}/libs/librocksdb.a
        -lpthread
        -lsnappy
        -ldl
        -lz
        )

target_link_libraries(store_node
        my_lib
        ${CMAKE_SOURCE_DIR}/libs/librocksdb.a
        -lpthread
        -lsnappy
        -ldl
        -lz
        )
