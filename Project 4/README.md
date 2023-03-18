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
assigned a unique encoded value. By using multiple threads, we can speed up how quickly we process input data into the binary tree
and thus how quickly we can encode data. After the thread finishes executing, it is written to the output file in the order it was
originally read in.

### Querying Methodology
The binary tree is used to massively speed up search operations. Specifically, once the dictionary has been encoded, we have a binary tree with every possible string entry loaded into it, along with an associated linked list structure that contains all the dictionary indices. A linked list was used specifically to deal with the issue of memory allocation, since a statically or dynamically allocated array would require a significant amount of memory rellocation, whereas a linked list can constantly add new points to newly allocated memory blocks. <br> <br>
There are two querying options: full-term lookup and prefix lookup, and each of these can use one of three methods: Vanilla file scanning, encoded dictionary lookup, and encoded dictionary lookup with SIMD instructions. Full-term lookup is done by running string comparions down the binary tree starting from the tree root. This process is O(log n) and is thus much faster than vanilla term lookups which are O(n). This process is further sped up by using SIMD instructions to do string comparions since multiple bits of data can be loaded into SIMD buffers and compared in parallel.

## Usage
Open a command terminal in a directory containing the `proj4.c`, `bin_tree.c`, and `bin_tree.h` files as well as your raw data file, e.g. `col.txt`, and use the commands below to compile and run the code. Note: you will have to install the `pthread` library if you do not have it, which can be done by entering the command: `sudo apt-get install libpthread-stubs0-dev`.

Compile line:
```
gcc proj4.c bin_tree.c -o proj4.out -msse4.1
```
If you cannot get your code to compile, then just run the included `proj4.out` executable file.

Execute line:
```
./proj4.out <input_file> <num_threads> <ssd_write_enable> <query_print_enable>
```
Arguments:<br>
`<input_file>`: The name of your input file, e.g. `col.txt`<br>
`<num_threads>`: The number of worker threads you would like to run for encoding.<br>
`<ssd_write_enable>`: Set this to `1` if you want to write data to the SSD, and `0` otherwise. <br>
&emsp; Note: `ssd_write_enable` should be only disabled when doing performance measurements, or if you don't want a dictionary file for some reason.
`<query_print_enable>`: Set this to `1` if you would like to enable index printing of your lookup strings, or `0` otherwise. <br>
&emsp; Note: `query_print_enable` should be only disabled when doing performance measurements or when encoding very querying very  large files since it significantly affects the program execution time.<br>

Here is an example execution command:
```
./proj4.out col.txt 4 1 1
```

Once the code is up and running, follow the instructions given by the terminal prompts. If you are using a large file for dictionary encoding, the encoding will take awhile (on my system, it took around 30 minutes using 4 threads). The reason it takes so long is to speed up the eventual search and scanning operations. This will be explained in more detail in the Analysis section.

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
For all tests, the following execution command was run:
```./proj4.out Column.txt <x> 0 0```
where `Column.txt` is a 1GB raw column data file and `<x>` was the desired number of threads.

### Encoding
| Num Threads | Encoding Time \(s\) |
|:-----------:|:-------------------:|
| 1           | 2725                |
| 2           | 000                 |
| 4           | 000                 |
| 8           | 000                 |

### Querying
#### Search term is 'pikgyaqet': <br>
|                          | Lookup Times \(s\) |
|:------------------------:|:------------------:|
| Vanilla Full-Term        | 4\.9               |
| Encoded Full-Term        | 0\.035             |
| Encoded + SIMD Full-Term | 0\.033             |

#### Search term is 'bruh': <br>
|                       | Lookup Times \(s\) |
|:---------------------:|:------------------:|
| Vanilla Prefix        | 5\.1               |
| Encoded Prefix        | 0\.19              |
| Encoded + SIMD Prefix | 0\.25              |


## Analysis
### Note from Patrick
This blurb is mainly for the professor. This code works as intended, but perhaps is not implemented exactly as was implied by the project description. Specifically, querying operations for the encoded dictionary are not done by scanning the encoded file, but rather by looking up values in the binary tree stored in memory. Initially, I had tried to do a scan of the encoded dictionary, but due to how I was doing file parsing (i.e. separating the encoded value from its associated string), it was always slower to query it. I instead opted to use the binary tree that I had generated to do querying, and once it had been implemented, I re-read the project description and realized this wasn't the intended way of doing querying. Ultimately, I've run out of time to work on this assignment due to time constraints from other classes and life activities, so I'm turning this in as is. You can still query very quickly, much faster than with vanilla searching, but there is no encoded file scanning involved.

### Explanation of Results
#### Encoding
Encoding takes *awhile*, and this is because every time a string is read from the dictionary, it is inserted into the binary tree (or rejected if it's already present). As a result, a lot of memory management is involved when encoding the dictionary. The benefit of doing this, however, is that string lookups become very quick since searching a binary tree is an O(log n) process, as opposed to an O(n) process, and you additionally avoid having to parse data in the file.<br><br>



#### Querying

## Conclusion
TBD
