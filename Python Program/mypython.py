#!/usr/bin/python
# Xiao Kuang
# 5/14/2019
# CS344 Python Program

# Your script must satisfy the following requirements:

# 1 Be contained in one single file, called "mypython.py".
# 2 When executed, create 3 files in the same directory as your script, each named differently (the name of the files is up to you), which remain there after your script finishes executing. Each of these 3 files must contain exactly 10 random characters from the lowercase alphabet, with no spaces ("hoehdgwkdq", for example). The final (eleventh) character of each file MUST be a newline character. Thus, running wc (wordcount) against your files in the following manner must return 11:
# 	$ cat myfile
# 		gkwjhcfikf
# 	$ wc -c myfile
# 		11 myfile
# 3 When executed, output sent to stdout should first print out the contents of the 3 files it is creating in exactly the format given below.
# 4 After the file contents of all three files have been printed, print out two random integers each on a separate line (whose range is from 1 to 42, inclusive).
# 5 Finally, on the last (sixth) line, print out the product of the two numbers.

import string
import random
from random import randint
# https://docs.python.org/3/library/random.html#random.randint


str1 = ''
str2 = ''
str3 = ''

# append random lowercase alphabet character to each string 10 times
for idx in range(10):
	str1 = "".join((str1, random.choice(string.ascii_lowercase)))
	str2 = "".join((str2, random.choice(string.ascii_lowercase)))
	str3 = "".join((str3, random.choice(string.ascii_lowercase)))

# program requires printing of strings in each file
print str1
print str2
print str3

# append newline to each string to satisfy program requirements
str1 = str1 + "\n"
str2 = str2 + "\n"
str3 = str3 + "\n"

# open 3 file objects for writing
f1 = open("file1.txt", "w+")
f2 = open("file2.txt", "w+")
f3 = open("file3.txt", "w+")

# write each string to each respective file
f1.write(str1)
f2.write(str2)
f3.write(str3)

# set random integers to two variables and get their product in a third variable
int1 = randint(1,49)
int2 = randint(1,49)
int3 = int1*int2

# print integers to satisfy program requirements
print int1
print int2
print int3
