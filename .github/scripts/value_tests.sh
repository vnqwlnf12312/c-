#! /bin/bash

script_path=./.github/scripts

for ((I=1; I<101; I=I+1))
do
    echo "Test $I"
    $script_path/value_test.sh
    if (($? != 0)); then
        exit 2
    fi
done