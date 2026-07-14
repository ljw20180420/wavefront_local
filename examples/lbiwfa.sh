#!/bin/bash

# change to the dir of the script
cd $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# change to the dir to the project
cd ..

g++ -O3 -std=gnu++23 \
    examples/example_lbiwfa.cpp \
    cpp/src/*.cpp \
    -o examples/example_lbiwfa

examples/example_lbiwfa
