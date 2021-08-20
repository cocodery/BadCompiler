cmake_minimum_required(VERSION 3.16)
project(compiler)

set(CMAKE_CXX_STANDARD 11)

add_executable(compiler main.cpp source/scanner.cpp include/scanner.h source/Parser.cpp include/Parser.h source/common.cpp include/common.h include/SymbolTable.h source/SymbolTable.cpp source/Optimization.cpp include/Optimization.h)