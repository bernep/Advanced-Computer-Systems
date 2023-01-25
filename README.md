# Advanced-Computer-Systems
A public repository for our projects in ECSE 6320: Advanced Computer Systems

### Project 1
## Overview
This project delves into multithreading and compression. Specifically, the proj1.c file takes some input file and compresses it into a .zst file (i.e. an archive file).

## Required Files
proj1.c
common.h
<input file>

## Usage
Open a command terminal in a directory containing all these files. Here are some example commands.
Compile line:
```
gcc proj1.c -lzstd -I/usr/include/zstd -L/usr/lib -pthread -o proj1.out
```
---> This will compile your code using the ZSTD and pthread libraries.

Execute line:
```
./proj1.out 10 my_input_video.mp4
```
---> Usage: ./proj1.out <num_threads> <input file>
---> num_threads specifies the number of worker threads you would like to generate

## Code Structure

## Experimental Results

## Analysis and Conclusion
