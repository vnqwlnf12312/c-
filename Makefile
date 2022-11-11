all: build

build:
	g++ -std=c++17 -g hello.cpp

test1:
	./.github/scripts/res_test.sh

memory_test:
	g++ -std=c++17 -g ./.github/cpp_tests/work_test.cpp
	./.github/scripts/memory_tests.sh;

work_test:
	g++ -std=c++17 -g ./.github/cpp_tests/work_test.cpp
	./a.out > out.txt;

allocs_test:
	g++ -std=c++17 -g ./.github/cpp_tests/allocs_test.cpp
	./a.out;

clean:
	rm a.out
	rm out.txt
	rm *.txt
