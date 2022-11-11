#! /bin/bash

script_path=./.github/scripts

python $script_path/generate_arrays.py > test.txt
A=$(head -n 1 test.txt)
tail -n +2 test.txt > test_without_sizes.txt
python $script_path/generate_answer.py > answer.txt && cat answer.txt >> answer1.txt
echo $A
