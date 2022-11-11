#! /bin/python3

import sys

with open ("out.txt") as file1:
    with open ("answer.txt") as file2:
        ans1 = int(file1.readline().strip())
        ans2 = int(file2.readline().strip())
        if (ans1 == ans2):
            sys.exit(0)
        else:
            print ("incorrect answer on test:")
            with open ("test_without_sizes.txt") as f:
                print(f.readlines())
            sys.exit(1)