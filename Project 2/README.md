## Project 2: Matrix-Matrix Multiplication with SIMD Instructions & Cache Miss Minimization
### Overview
This project showcases the performance differences between standard C instructions and [Intel's intrinsic SIMD x86 library](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html).

### Usage
Open a command terminal in a directory containing the `proj2.c` file, and use the commands below to compile and run the code.
```
proj2.c
``` 

Compile line:
```
gcc proj2.c -o proj2.out -mavx2 -mavx512bw -mavx512vl
```
Assuming you're using relatively modern x86 processor, you should have all of these libraries already. However, if you cannot get your code to compile, then just run the included `proj2.c` executable file.

Execute line:
```
./proj2.out <n> <seed> <-arg1> <-arg2>
```
Arguments:<br>
`<n>`: Size of input and output matrices (positive integer). The matrices will be of size n-by-n.<br>
`<seed>`: Randomizer seed (positive integer). This specifies the internal C randomizer seed.<br>
`<-arg1>`: Specifies which data type and instruction set to use.<br>
&emsp;`-fo` --> 4-byte floating-point data, SIMD instructions<br>
&emsp;`-fu` --> 4-byte floating-point data, C instructions<br>
&emsp;`-xo` --> 2-byte fixed-point data, SIMD instructions<br>
&emsp;`-xu` --> 2-byte fixed-point data, C instructions<br>
`<-arg2>`: Specifies print options (this argument is optional)<br>
&emsp;`-p` --> Print progress while computing matrices<br>
&emsp;`-m` --> Print input and output matrices<br>
&emsp;`-pm` --> Print progress and matrices<br>

Here is an example execute command:
```
./proj2.out 10 10 -fo -pm
```

Be careful not to print matrices (i.e. `-m` or `-pm`) when making `n` large. The program will significantly slow down and the output will be difficult to read. Additionally, progress (i.e. `-p`) will not be a very helpful metric for small matrices, as it's meant to display progress for matrices of size 1000 or greater.

### Code Structure
The `proj2.c` file has plenty of documentation explaining what is happening step-by-step, but the following is more of a high-level overview.

The program begins by taking the input arguments given by the user and interpreting them. Specifically, a random matrix of size `n`-by-`n` is allocated into memory and populated with random values, which is determined by the `seed` argument. Matrices are generated on the fly. Specifically, a single matrix is generated and then multiplied against itself (so the output matrix is the square of the input matrix). The matrix multiplication functions were all designed to be able to multiply any two matrices, regladless if they are the same one or not, but for the sake of reducing overhead, only one input matrix was used for this program.

After initializaiton is complete, the code will read the given data-type and algorithm argument (i.e. `-arg1`) and step into one of four paths for matrix computation, as there are four separate matrix computation functions (4-byte floating-point data, optimized multiplication, 4-byte floating-point data, unoptimized multiplication, 2-byte fixed-point data, optimized multiplication, 2-byte fixed-point data, unoptimized multiplication). The matrices are multiplied together, and the output is stored into an output matrix. Afterwards, a memory cleanup is performed and the program ends.

There are also checks for prints throughout the code if the user wishes to read additional information about the matrices (i.e. their contents or the progress in computing said matrices). See the Usage section for more details on how to print data.

### Hardware Environment
The CPU used to run this code was an Intel® Core™ i7-1165G7 Processor with the following specs: <br>
---> 4 cores <br>
---> 8 threads <br> 
---> 2.8 GHz clock <br>
---> 12 MB cache <br>

### Experimental Results

Data was collected for the purposes of comparing the efficiency of the unoptimized algorithms and the SIMD based optimized algorithms. During the data sampling process, the time taken for a matrix multiplication on both optimized and unoptimized algorithms for both the fixed point and floating point values was taken. When data was collected, the fixed seed 10 was used, and the matrix sizes of 100, 200, 500, 1000, 2000, and 5000 were used. Due to the long length of time, the time taken for the multiplication of 10000 size matrix was only recorded for the optmized algorithms. The results of such experiments can be seen on the graphs below.

Unoptimized Fixed Point

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%202/Time%20(Seconds)%20vs.%20Matrix%20Size%20For%20Unoptimized%20Fixed%20Point.png)

Optimized Fixed Point

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%202/Time%20(Seconds)%20vs.%20Matrix%20Size%20for%20Optimized%20Fixed%20Point.png)

Unoptimized Floating Point

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%202/Time%20(Seconds)%20vs.%20Matrix%20Size%20for%20Unoptimized%20Floating%20Point.png)

Optimized Floating Point

![alt text](https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%202/Time%20(Seconds)%20vs.%20Matrix%20Size%20For%20Optimized%20Floating%20Point.png)

### Analysis and Conclusion

As we can see on the graphs found above, the optimized algorithms take much less time than the unoptimized algorithms (although, since we skipped the 10000-size matrix for unoptimized computation, the graphs look somewhat misleading; look closely at the x-axis to see the performance improvements when comparing graphs). When comparing the time taken by the matrix multiplication of a 100 size fixed point matrix by the optimized and unoptimized algorithms, the time taken is not much different, with the optimized algorithm taking ~94% of the unoptimized algorithm time. Similarly the improvement found on multiplication of a 100 size floating point matrix by the optimized and unoptimized algorithms is not very large, with the optimized algorithm taking ~85% of the unoptimized algorithm's time. When comparing larger values, we can see that the improvement is very large. For a 5000 size fixed point matrix, when comparing the matrix multiplication time taken by the optimized and unoptimized code, we can see that the optimized code only takes 36.18277219% of the time taken by the unoptimized algorithm. Likewise, when looking at the 5000 size floating point matrix, we can see that the optmized algorithm runtime improvement is very good, with the optmized runtime being 27.31516057% of the original unoptimized algorithm.

In conclusion, we can see that on a large scale, the use of SIMD commands speeds up functions by a large amount.

### Troubleshooting
This program was run on a native Ubuntu 22.04 LTS installation. When trying this code on a Windows 11 laptop, there were execution errors. If you have issues running this code, please contact me (Patrick Berne) over Webex.
