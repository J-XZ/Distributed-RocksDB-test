cmake_minimum_required(VERSION 3.21)
project(Distributed_RocksDB)

set(CMAKE_CXX_STANDARD 14)

add_executable(store_node store_node.cpp defs.h)

add_executable(control_node control_node.cpp defs.h)
