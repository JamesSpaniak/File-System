File System
Author: James Spaniak

To run program:
run make

then ./driver -f file_list.txt -d dir_list.txt -s 1600000 -b 16

Where the two files come from:
find ./ -type d > dir_list.txt
find ./ -type f -ls > file_list.txt

-s: disk size
-b block size


Then the following commands are available:

• cd [directory] - set specified directory as the current directory

• cd.. - set parent directory as current directory

• ls - list all files and sub-directories in current directory

• mkdir [name] - create a new directory in the current directory

• create [name] - create a new file in the current directory

• append [name] [bytes] - append a number of bytes to the file

• remove [name] [bytes] - delete a number of bytes from the file

• delete [name] - delete the file or directory

• exit - deallocate data structures and exit program

• dir - print out directory tree in breadth-first order

• prfiles - print out all file information

• prdisk - print out disk space information