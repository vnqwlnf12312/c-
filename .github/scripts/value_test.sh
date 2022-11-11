#! /bin/bash

script_path=./.github/scripts

A=$($script_path/res_test.sh)
./a.out $A < test_without_sizes.txt > out.txt
python $script_path/check_answer.py
exit $?