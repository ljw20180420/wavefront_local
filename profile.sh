#!/bin/bash

# change to the dir of the script
cd $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

compile_lwfa() {
    g++ -O3 -std=gnu++23 \
        cpp/benchmark/benchmark_lwfa.cpp \
        -o cpp/benchmark/benchmark_lwfa
}

compile_lbiwfa() {
    g++ -O3 -std=gnu++23 \
        cpp/benchmark/benchmark_lbiwfa.cpp \
        -o cpp/benchmark/benchmark_lbiwfa
}

# compile_lwfa
# perf record -g cpp/benchmark/benchmark_lwfa 1000 100 100
compile_lbiwfa
perf record -g -F 10000 cpp/benchmark/benchmark_lbiwfa 1000 100 100
