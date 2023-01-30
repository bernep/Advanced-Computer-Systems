# Advanced-Computer-Systems
A public repository for our projects in ECSE 6320: Advanced Computer Systems

## Project 1: Compression and Multithreading
### Overview
This project heavily employs the functions of the `pthread` and `zstd` libraries. Specifically, the `proj1.c` file takes some input file and compresses it into a `.zst` file (i.e. an archive file) using a specified number of worker threads.

### Usage
Open a command terminal in a directory containing the project files, and use the commands below to compile run the code. The necessary files you will need are:
```
proj1.c
common.h
<input file>
``` 

Compile line:
```
gcc proj1.c -lzstd -I/usr/include/zstd -L/usr/lib -pthread -o proj1.out -fno-stack-protector
```
This will compile your code using the ZSTD and pthread libraries (assuming you have them installed). In case you cannot get the compiler to work on your system, you can just run the included `proj1.out` file using the execution line below. As a note, `-fno-stack-protector` may not be needed on your system. On the three machines we tested, it was needed on one of them due to a memory management issue, although this only occurred when either the input file was excessively large or too many threads were called on.

Execute line:
```
./proj1.out <num_threads> <input_file>
```
The arguments are `<number of worker threads you would like to run>` and `<your input file>`. Once run, you will receive your input file as a compressed `.zst` file in the same directory. Make sure the file is allowed to run as an executable (which can be changed by going into the file's properties), otherwise your system may not let you run it. <br />
Here is an example execute command:
```
./proj1.out 10 ds9.txt
```

If you don't have an archive extraction program installed, you can use ZSTD's `unzstd` function to uncompress the file for you. Using the above input file as an example, you can run the following command:
```
unzstd ds9.txt.zst
```

### Code Structure
The `proj1.c` file has plenty of documentation explaining what is happening step-by-step, but the following is more of a high-level overview.

Main function:
1) File and data initialization
2) For each thread specified by the user, read input data as a 16kB block, and send that data block to a worker thread to be compressed.
3) Whenever one thread ends, write its output data to an output file, and then read another input data block into a new thread.
4) Once all threads have been exhausted and all data has been written, do some memory cleanup and end the program.

Worker thread:
1) Receive a struct that contains a 16kB input data block.
2) Create an output buffer on that struct to store the compressed data.
3) Use ZSTD to compress the data.
4) Perform memory cleanup and end the thread. The struct with the compressed data is stored elsewhere and can be accessed outside of the thread.

### Hardware Environment
The CPU used to run this code was an Intel® Core™ i7-1260P Processor with the following specs:
---> 12 cores
---> 16 threads
---> 2.5GHz clock
---> 18MB cache

### Experimental Results
For the purposes of testing and data collection, a test file, `video.mp4` with 36.48 Megabytes, was used. We collected the time taken by the algorithm to compress the test file, under the conditions of 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, and 50 threads. The data below was collected through experimentation, and the graph was formatted in Excel. The time recorded is the time taken by the entire program to run.

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%201/results.png)

When testing this on larger files (i.e. 1 GB or larger), the experimental curve remains about the same. The time to process the input file scales in a roughly O(n) fashion depending on file size.

### Analysis and Conclusion

As we can see on the graph above, it's ideal to use at least 5 worker threads. The specific times for different thread allotments are 0.33 seconds (1 thread), 0.23 seconds (2 threads), 0.21 seconds (3 threads), and 0.17 seconds (4 threads). When 5 or more threads are used, the compression time stabilizes around 0.16 seconds, even when increasing the number of threads to upwards of 50 threads. A possible reason for this may be that the time it takes to set up the threads becomes more than the amount of time it take for the next thread in the queue to finish compression.

Thus, we can see that although increasing the number of threads does make total compression time smaller, increasing the number of threads leads to diminishing returns. Additionally, fewer threads leads to less memory being used anyway, so there's no reason to overdo the number of threads since too many threads can cause memory overflow. A balance should be struck between the number of threads and processing time.
