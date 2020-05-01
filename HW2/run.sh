#!/bin/bash




echo "Please enter interval_min interval_max process_n thread_n"
echo "Example arguments:101 200 2 2"
read int_min int_max np nt

gcc -o program Homework2.c -pthread
./program $int_min $int_max $np $nt
