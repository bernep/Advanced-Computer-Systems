# Advanced-Computer-Systems
A public repository for our projects in ECSE 6320: Advanced Computer Systems

## Project 1: Compression and Multithreading
### Overview
This project heavily employs the functions of the `pthread` and `zstd` libraries. Specifically, the proj1.c file takes some input file and compresses it into a .zst file (i.e. an archive file) using a specified number of worker threads.

### Usage
Open a command terminal in a directory containing the project files, and use the commands below to compile run the code. The necessary files you will need are:
```
proj1.c
common.h
<input file>
``` 

Compile line:
```
gcc proj1.c -lzstd -I/usr/include/zstd -L/usr/lib -pthread -o proj1.out
```
This will compile your code using the ZSTD and pthread libraries.

Execute line:
```
./proj1.out <num_threads> <input_file>
```
The arguments are `<number of worker threads you would like to run>` and `<your input file>`
```
./proj1.out 10 ds9.txt
```

Uncompress line (to test that the file is uncorrupted):
```
unzstd ds9.txt.zst
```

### Code Structure
The proj1.c file has plenty of documentation explaining what is happening step-by-step, but a high level overview looks like this:
1) File and data initialization
2) For each thread specified by the user, read input data as a 16kB block, and send that data block to a threading function to be compressed.
3) Whenever one thread ends, write its output data to an output file, read another input data block, and start another thread.
4) Once all threads have been exhausted and all data has been written, do some memory cleanup and end the program.

And additionally, the threading function overview is as follows:
1) Receive a struct that contains a 16kB input data block.
2) Create an output buffer on that struct to store the compressed data.
3) Use ZSTD to compress the data.
4) Perform memory cleanup and end the thread. The struct with the compressed data is stored elsewhere and can be accessed outside of the thread.

### Experimental Results

### Analysis and Conclusion
