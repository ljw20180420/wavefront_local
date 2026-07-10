#!/bin/bash

# change to the dir of the script
cd $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# change to the dir to the project
cd ..

parse_ini() {
    local target=$1
    sed -nEe '
        /^'${target}'/{
            s/^'${target}' *=//
            s/,//g
            p
        }
    ' paper/benchmark.ini
}

benchmark_lwfa() {
    local lengths=$1
    local err_nums=$2
    local end_ups=$3

    g++ -O3 -std=gnu++23 \
        cpp/benchmark/benchmark_lwfa.cpp \
        cpp/src/*.cpp \
        -o cpp/benchmark/benchmark_lwfa

    mkdir -p paper/benchmark/result
    printf "length,err_num,end_up,time,memory\n" \
        > paper/benchmark/result/lwfa.csv
    for length in ${lengths}
    do
        for err_num in ${err_nums}
        do
            for end_up in ${end_ups}
            do
                cpp/benchmark/benchmark_lwfa ${length} ${err_num} ${end_up} >> paper/benchmark/result/lwfa.csv
            done
        done
    done
}

benchmark_lbiwfa() {
    local lengths=$1
    local err_nums=$2
    local end_ups=$3

    g++ -O3 -std=gnu++23 \
        cpp/benchmark/benchmark_lbiwfa.cpp \
        cpp/src/*.cpp \
        -o cpp/benchmark/benchmark_lbiwfa

    mkdir -p paper/benchmark/result
    printf "length,err_num,end_up,time,memory\n" \
        > paper/benchmark/result/lbiwfa.csv
    for length in ${lengths}
    do
        for err_num in ${err_nums}
        do
            for end_up in ${end_ups}
            do
                cpp/benchmark/benchmark_lbiwfa ${length} ${err_num} ${end_up} >> paper/benchmark/result/lbiwfa.csv
            done
        done
    done
}

benchmark_parasail() {
    local lengths=$1
    local err_nums=$2
    local end_ups=$3
    local method=$4

    g++ -O3 -std=gnu++23 \
        cpp/benchmark/benchmark_parasail.cpp \
        cpp/src/*.cpp \
        -L./cpp/parasail/build \
        -lparasail \
        -Wl,-rpath,'$ORIGIN/../parasail/build' \
        -o cpp/benchmark/benchmark_parasail

    mkdir -p paper/benchmark/result
    printf "length,err_num,end_up,time,memory\n" \
        > paper/benchmark/result/${method}.csv
    for length in ${lengths}
    do
        for err_num in ${err_nums}
        do
            for end_up in ${end_ups}
            do
                cpp/benchmark/benchmark_parasail ${length} ${err_num} ${end_up} ${method} >> paper/benchmark/result/${method}.csv
            done
        done
    done
}

benchmark_seqan() {
    local lengths=$1
    local err_nums=$2
    local end_ups=$3

    g++ -O3 -std=c++23 \
        -Icpp/include \
        cpp/benchmark/benchmark_seqan.cpp \
        cpp/src/*.cpp \
        -o cpp/benchmark/benchmark_seqan

    mkdir -p paper/benchmark/result
    printf "length,err_num,end_up,time,memory\n" \
        > paper/benchmark/result/seqan.csv
    for length in ${lengths}
    do
        for err_num in ${err_nums}
        do
            for end_up in ${end_ups}
            do
                cpp/benchmark/benchmark_seqan ${length} ${err_num} ${end_up} >> paper/benchmark/result/seqan.csv
            done
        done
    done
}


lengths=$(parse_ini length)
err_nums=$(parse_ini err_num)
end_ups=$(parse_ini end_up)
benchmark_lwfa "${lengths}" "${err_nums}" "${end_ups}"
benchmark_lbiwfa "${lengths}" "${err_nums}" "${end_ups}"
benchmark_parasail "${lengths}" "${err_nums}" "${end_ups}" diag
benchmark_parasail "${lengths}" "${err_nums}" "${end_ups}" striped
benchmark_parasail "${lengths}" "${err_nums}" "${end_ups}" scan
benchmark_seqan "${lengths}" "${err_nums}" "${end_ups}"
