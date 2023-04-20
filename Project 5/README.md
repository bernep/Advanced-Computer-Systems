# Project 5: Video Processing using Multiple Threads and SIMD Parallelization

## Setup Instructions
Follow OpenCV setup here: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html

Also nice to have: 
```
sudo apt update
sudo apt install ffmpeg
ffmpeg -version
```

Any .mp4 file will work. You can download one from anywhere. You can also specify the number of frames to process so that you do not have to process the entire video.

## Usage
Open a command terminal in a directory containing the `proj5.h`, `proj5.cpp`, `gaussian_blur.cpp`, `grayscale.cpp`, and `edge_detection.cpp` files as well as your `<filename>.mp4` file, e.g. `my_video.mp4`, and use the commands below to compile and run the code.

Compile line:
```
g++ -std=c++11 proj5.cpp gaussian_blur.cpp edge_detection.cpp grayscale.cpp -o proj5.out -lopencv_core -lopencv_videoio -lopencv_imgproc -I/usr/include/opencv4
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

Once the code is up and running, follow the instructions given by the terminal prompts. If you are using a large file for dictionary encoding (e.g. the file that the professor provided), the encoding will take awhile (on my system, it took around 25 minutes using 4 threads). The reason it takes so long is to speed up the eventual search and scanning operations. This will be explained in more detail in the Analysis section.

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
