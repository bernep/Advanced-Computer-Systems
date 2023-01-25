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
For the purposes of testing and data collection, a test file, video.mp4 with 36.48 Megabytes, was used. We collected the time taken by the algorithm to compress the test file, under the conditions of 1,2,3,4,5,6,7,8,9,10,15,20,30,40, and 50 threads. By inputting such data on excel, the graph below was created. The time recorded is the time taken by the entire program to run.

![alt text] https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%201/results.png

### Analysis and Conclusion

As we can see on the graph above, when using a number of threads smaller than 5, the compression time decreases fastly, with the compression time for 1 thread being around 0.33 seconds, for 2 threads being 0.23 seconds, 3 threads being 0.21 seconds, and 4 threads being 0.17 seconds. When 5 or more threads are used, the compression time stabilizes around 0.16 seconds, even when increasing the number of threads to upwards of 50 threads. A possible reason for this is the fact that 0.16 seconds might be the time taken for setting up the threads, and the time actually taken by the zstd compression would become very small when 5 threads or more are being used.

Thus, we can see that although increasing the number of threads does make total compression time smaller, there is an optimal number of threads being used where any more than that would not lead to any significant decrease in time.
