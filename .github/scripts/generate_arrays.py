#! /bin/python

import random

result = []
k = random.randint(2,6)
for i in range (0,k):
    result.append ([])
    for j in range (0, random.randint(i+1,10)):
        result[-1].append (random.randint(-100,100))

print (' '.join ([str(len(i)) for i in result]))
for i in result:
    print (' '.join ([str(j) for j in i]))
