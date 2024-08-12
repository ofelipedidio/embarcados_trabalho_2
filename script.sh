#!/bin/bash

do_thing() {
    N="$1"
    echo "#define TEST_CYCLES $N" > src/params.h
    sudo make clean bin/program
    echo -e "\033[31mRUNNING WITH N=$N\033[0m"
    ./bin/program 2> err.txt
    ./bin/program 2> err.txt
    ./bin/program 2> err.txt
    ./bin/program 2> err.txt
    ./bin/program 2> err.txt
}

do_thing 100
do_thing 200
do_thing 300
do_thing 400
do_thing 500
do_thing 1000
