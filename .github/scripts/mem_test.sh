#! /bin/bash

script_path=./.github/scripts

valgrind --log-file=./valgrind_log.txt ./a.out > out.txt
Total_memory=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $9}' | sed 's/,//g')
let Total_memory=$Total_memory-76800
Total_allocs=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $5}' | sed 's/,//g')
Total_free=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $7}' | sed 's/,//g')

if (( Total_allocs > Total_free))
then
    echo "Мне тут valgrind на ушко шепнул, что не все new нашли себе в пару delete, а жаль("
    exit 1
fi;