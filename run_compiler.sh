#!/bin/bash

# 设置环境变量忽略内存泄漏检查
export ASAN_OPTIONS="detect_leaks=0:print_suppressions=0:halt_on_error=1:abort_on_error=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=1:abort_on_error=1"

# 运行编译器
./compiler "$@"
