# Advanced-Computer-Systems
A public repository for our projects in ECSE 6320: Advanced Computer Systems

## Project 1: Compression and Multithreading
### Overview
This project heavily employs the functions of the `pthread` and `zstd` libraries. Specifically, the `proj1.c` file takes some input file and compresses it into a .zst file (i.e. an archive file) using a specified number of worker threads.

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
This will compile your code using the ZSTD and pthread libraries. In case you cannot get the compiler towork on your system, you can just run the included `proj1.out` file using the execution line below.

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

### Experimental Results
For the purposes of testing and data collection, a test file, `video.mp4` with 36.48 Megabytes, was used. We collected the time taken by the algorithm to compress the test file, under the conditions of 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 30, 40, and 50 threads. The graph below was collected and formatted in Excel. The time recorded is the time taken by the entire program to run.

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%201/results.png)

When testing this on larger files (i.e. 1 GB or larger), the experimental curve remains about the same, and the time to process the input file scales in a roughly O(n) fashion.

### Analysis and Conclusion

As we can see on the graph above, it's ideal to use at least 5 worker threads. The specific times for different thread allotments are 0.33 seconds (1 thread), 0.23 seconds (2 threads), 0.21 seconds (3 threads), and 0.17 seconds (4 threads). When 5 or more threads are used, the compression time stabilizes around 0.16 seconds, even when increasing the number of threads to upwards of 50 threads. A possible reason for this may be that the time it takes to set up the threads becomes more than the amount of time it take for the next thread in the queue to finish compression.

Thus, we can see that although increasing the number of threads does make total compression time smaller, there is an optimal number of threads that can be used where any more than that would not lead to any significant decrease in time. Additionally, fewer threads leads to less memory being used anyway, so there's no reason to overdo the number of threads being used.
