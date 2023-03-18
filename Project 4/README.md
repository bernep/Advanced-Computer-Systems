# Project 4: Implementation of Dictionary Codec using Multiple Threads, a Binary Tree, and SIMD Instruction Lookup
## Overview and Code Structure
### Introduction
In this project, we implement a dictionary codec. This is a way of compressing a file by encoding words with smaller data,
e.g. the string "FunnyWord" can be encoded with the the value '123', which is fewer bits than the original string. By doing this,
you can end up with a file that takes up less space, and data is easier to scan due to its reduced compllexity.

### Encoding Methodology
Encoding is done through the use of a binary tree and multiple worker threads. When data is read from the input file, it is 
continously passed to a worker thread. In these worker threads, values are assigned a code (initially 1, and it increases
for each unique string) and these values, along with their codes, are placed into the binary tree. The benefit of multithreading
in this case is populating and searching the binary tree when encoding. Whenever a value is passed to a worker thread, the value
is inserted into the binary tree (and accepted if it's unique, otherwise it's rejected), and it is successfully inserted, it is
assigned a unique encoded value. By using multiple threads, we can speedup how quickly we process input data into the binary tree
and thus how quickly we can encode data. After the thread finishes executing, it is written to the output file in the order it was
originally read in.

### Querying Methodology
The binary tree is used to massively speed up search operations. Specifically, once the dictionary has been encoded, we have a
binary tree with every possible string entry loaded into it, along with an associated linked list structure that contains all
the dictionary indices. A linked list was used specifically to deal with the issue of memory allocation, since a statically or
dynamically allocated array would require a significant amount of memory rellocation, whereas a linked list can constantly add
new points to newly allocated memory blocks. <br> <br>
There are querying options: full-term lookup and prefix lookup. Full-term lookup is done by running string comparions down the
binary tree starting from the tree root. This process is O(log n) and is thus much faster than vanilla term lookups which are
O(n). This process is further sped up by using SIMD instructions to do string comparions since multiple bits of data can be
loaded into SIMD buffers and compared in parallel.

## Usage
Open a command terminal in a directory containing the `proj4.c`, `bin_tree.c`, and `bin_tree.h` files as well as your raw data file, e.g. `col.txt`, and use the commands below to compile and run the code. Note: you will have to install the `pthread` library if you do not have it, which can be done by entering the command: `sudo apt-get install libpthread-stubs0-dev`.

Compile line:
```
gcc proj4.c bin_tree.c -o proj4.out -msse4.1
```
If you cannot get your code to compile, then just run the included `proj4.out` executable file.

Execute line:
```
./proj4.out <input_file> <num_threads> <query_print_enable>
```
Arguments:<br>
`<input_file>`: The name of your input file, e.g. `col.txt`<br>
`<num_threads>`: The number of worker threads you would like to run for encoding.<br>
`<query_print_enable>`: Set this to '1' if you would like to enable index printing of your lookup strings.<br>
&emsp;`-fo` Note: `query_print_enable` should be disabled when doing performance comparisons since it significantly
                  affects the program execution time.<br>

Here is an example execute command:
```
./proj4.out col.txt 5 1
```

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
TBD

## Analysis
TBD

## Conclusion
TBD
