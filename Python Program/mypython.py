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
