#! /bin/python3

import re

with open ("BigInteger.h") as file:
    BI_begin_flag = False
    R_begin_flag = False
    friend_counter = 0

    for line in file:
        found1 = re.match ("\s*class\s*BigInteger.*", line)
        found2 = re.match ("\s*class\s*Rational.*", line)
        if (found1 != None):
            BI_begin_flag = True
            #print (line)

        if (found2 != None):
            R_begin_flag = True
            BI_begin_flag = False
            if (friend_counter > 4):
                exit ("Too many friends in BigInteger. Should be less than 5")
            #print (str(friend_counter) + " ... " + line)
            friend_counter = 0
        
        if ((BI_begin_flag or R_begin_flag) and line.find("friend") != -1):
            friend_counter+=1
    #print (friend_counter)
    if (friend_counter > 2):
        exit("Too many friends in Rational. Should be less than 3")