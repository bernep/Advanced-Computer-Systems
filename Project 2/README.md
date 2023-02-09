## Project 2: Matrix-Matrix Multiplication with SIMD Instructions & Cache Miss Minimization
### Overview
This project showcases the performance differences between standard C instructions and [Intel's intrinsic SIMD x86 library](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html).

### Usage
Open a command terminal in a directory containing the project files, and use the commands below to compile run the code. The necessary files you will need are:
```
proj1.c
``` 

Compile line:
```
gcc proj2.c -o proj2.out -mavx2 -mavx512bw -mavx512vl
```
Assuming you're using relatively modern x86 processor, you should have all of these libraries already. However, if you cannot get your code to compile, then just run the included executable file.

Execute line:
```
./proj2.out <n> <seed> <-arg1> <-arg2>
```
Arguments:<br>
`<n>`: Size of input matrix (integer). This matrix will be of size n-by-n, and the output matrix will be equivalent to the square of input matrix.<br>
`<seed>`: Randomizer seed (integer). This is a non-negative integer value that controls the internal C randomizer used by the program.<br>
`<-arg1>`: Specifies which data type and instruction set to use.
&emsp `-fo` --> 4-byte floating-point data, SIMD instructions<br>
&emsp `-fu` --> 4-byte floating-point data, C instructions<br>
&emsp `-xo` --> 2-byte fixed-point data, SIMD instructions<br>
&emsp `-xu` --> 2-byte fixed-point data, C instructions<br>
`<-arg2>`: Specifies print options (this argument is optional)
&emsp `-p` --> Print progress while computing matrices<br>
&emsp `-m` --> Print input and output matrices<br>
&emsp `-pm` --> Print progress and matrices<br>

Here is an example execute command:
```
./proj2.out 10 10 -fo -pm
```

### Code Structure
The `proj2.c` file has plenty of documentation explaining what is happening step-by-step, but the following is more of a high-level overview.

TBD

### Hardware Environment
The CPU used to run this code was an `TBD` with the following specs: <br>
---> `TBD` cores <br>
---> `TBD` threads <br>
---> `TBD` GHz clock <br>
---> `TBD` MB cache <br>

### Experimental Results
TBD

### Analysis and Conclusion
TBD
