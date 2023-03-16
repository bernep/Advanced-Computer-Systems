# Project 4: Implementation of Dictionary Codec using Multiple Threads, a Binary Tree, and SIMD Instruction Lookup
## Overview and Code Structure
### Introduction
In this project, we implement a dictionary codec. This is a way of compressing a file by encoding words with smaller data,
e.g. the string "FunnyWord" can be encoded with the the value '123', which is fewer bits than the original string. By doing this,
you can end up with a file that takes up less space, and data is easier to scan due to its reduced compllexity.

### Encoding Methodology
Encoding is done through the use of a binary tree and multiple worker threads. When data is read from the input file, it is continously
passed to a worker thread. In these worker threads, values are assigned a code (initially 1, and it increases for each unique string)
and these valuse, along with their codes, are placed into the binary tree. The benefit of multithreading in this case is populating and 
searching the binary tree when encoding. Whenver a value is passed to a worker thread, the value is inserted into the binary tree
(and accepted if it's unique, otherwise it's rejected), and it is successfully inserted, it is assigned a unique encoded value. By
using multiple threads, we can speedup how quickly we process input data into the binary tree and thus how quickly we can encode
data. After the thread finishes executing, it is written to the output file in the order it was originally read in.

### Querying Methodology
TBD

## Testing Environment
#### System
* Framework Laptop
* WSL2 - Ubuntu 22.04 LTS

#### CPU
* Intel® Core™ i7-1165G7 Processor
  - 4 cores
  - 8 threads
  - 2.8 GHz clock
  - 12 MB cache (L1: 96K, L2: 1.25MB, L3: 12MB)
  
#### Memory
* DDR4 SO-DIMM
* 3200 MHz
* 32GB of Memory

#### Storage
* Vansuny USB 3.1 Flash Drive
* 128 GB of Storage
* 400 MB/s Read Speed
* 240 MB/s Write Speed


## Experimental Results
### Testing Procedure
TBD

### Results
TBD

## Analysis
TBD

## Conclusion
TBD
