all: build

memory_test:
	g++ -std=c++17 -g ./.github/cpp_tests/work_test.cpp
	./.github/scripts/memory_tests.sh;

work_test:
	g++ -std=c++17 -g ./.github/cpp_tests/work_test.cpp
	./a.out > out.txt;

exception_test:
	g++ -std=c++17 -g ./.github/cpp_tests/exception_test.cpp
	./a.out > out_except.txt;

clean:
	rm a.out
	rm out.txt
	rm *.txt
