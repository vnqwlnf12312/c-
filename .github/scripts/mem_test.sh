#! /bin/bash

script_path=./.github/scripts

A=$($script_path/res_test.sh)
elements_count=0
for i in $A; do
  let elements_count+=$i
done

valgrind --error-exitcode=3 --exit-on-first-error=yes --log-file=./valgrind_log.txt ./a.out $A < test_without_sizes.txt > out.txt
if (($? != 0)) 
then
	echo "Кажется, valgrind что-то нашёл"
	echo "Тест:"
    cat test_without_sizes.txt
	cat valgrind_log.txt
	exit 3
fi;


Total_memory=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $9}' | sed 's/,//g')
let Total_memory=$Total_memory-80896
Total_allocs=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $5}' | sed 's/,//g')
Total_free=$(cat valgrind_log.txt | grep 'total heap usage' | awk -F ' ' '{print $7}' | sed 's/,//g')

if (( Total_allocs > Total_free))
then
    echo "Кто-то память не освободил. Кто бы это мог быть?"
    echo "Тест:"
    cat test_without_sizes.txt
    exit 1
fi;

if ((Total_memory > elements_count*10*4))
then
    echo "ЭЭ, куда столько памяти? Зачем тебе $Total_memory байт на $elements_count элементов?"
    echo "Другим хоть чуть оставь. Зачем столько выделять?"
    echo "Тест:"
    cat test_without_sizes.txt
    exit 2
fi;