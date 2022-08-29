In this program, you will be given an incomplete C++ program, which you need to finish. The code is supposed to
recursively scan for all files and directories and then report various statistics about the files and directories it encounters.
To finish the code, you will need to use several different system calls and some useful standard C++ containers.
The code you will write will include the following functionality:
• Recursive scanning of files and subdirectories.
• Determining the sizes of all files;
• Finding duplicate files;
• Finding most common file types;
• Finding most common words in all files.
Please note that you do not have to write recursive code – but you do need to write code that discovers all subdirectories,
sub-subdirectories, sub-sub-subdirectories, etc. Recall some of the iterative techniques you used in earlier courses to
traverse trees.

# dirStats - skeleton for Assignment 2

To compile all code, type:
```
$ make
```

To run the resulting code on directory test1:
```
$ ./dirstats test1
```
## IMPORTANT

Only modify and submit the `getDirStats.cpp` file. Your TAs will
supply their own versions of the other files (such as main.cpp) to
compile and test your code.


