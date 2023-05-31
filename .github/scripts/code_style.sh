#! /bin/bash

script_path=./.github/cpp_tests

#$script_path/friend_counter_checker.py
g++ -std=c++17 -g ./.github/cpp_tests/has_operators.cpp
./a.out
