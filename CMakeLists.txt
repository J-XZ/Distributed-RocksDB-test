cmake_minimum_required(VERSION 3.10)
project(Distributed_RocksDB)

set(CMAKE_CXX_STANDARD 14)
include_directories(include)

add_executable(store_node store_node.cpp defs.h)
target_link_libraries(store_node ${CMAKE_SOURCE_DIR}/libs/librocksdb_debug.a -lpthread -lsnappy -ldl -lz)

add_executable(control_node control_node.cpp defs.h)
