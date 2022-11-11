#! /bin/python3

import itertools
import numpy as np

arrays = []
maxLen = 0
with open("test_without_sizes.txt") as file:
    for line in file:
        arrays.append( [int(i) for i in line.strip().split(' ')])
        maxLen = max(maxLen, len(arrays[-1]))
    pass

indices = list (itertools.product (*[range(len(i)) for i in arrays]))
indices = [i for i in indices if len(set(i)) == len(i)]

npArrays = np.array (list (itertools.zip_longest (*arrays, fillvalue=0)), dtype=np.int32)

s = list(range(len(arrays)))
numbers = [np.prod (npArrays [i,s]) for i in indices]
print (np.sum (numbers), end='')
