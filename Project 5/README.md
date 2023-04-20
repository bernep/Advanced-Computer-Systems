# Project 5: Video Processing using Multiple Threads and SIMD Parallelization

## Setup Instructions
1) OpenCV is needed to run this program. Please follow the isntructions for OpenCV setup here: https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html
2) `ffmpeg` is not technically required, but without it, the output video will not have any audio. On Debian systems, this can be installed with the following commands:
```
sudo apt update
sudo apt install ffmpeg
ffmpeg -version
```

For other systems, please do your own research.

## Usage
Open a command terminal in a directory containing the `proj5.h`, `proj5.cpp`, `gaussian_blur.cpp`, `grayscale.cpp`, and `edge_detection.cpp` files as well as your `<filename>.mp4` file, e.g. `my_video.mp4`, and use the commands below to compile and run the code.

Compile line:
```
g++ -std=c++11 proj5.cpp gaussian_blur.cpp edge_detection.cpp grayscale.cpp -o proj5.out -lopencv_core -lopencv_videoio -lopencv_imgproc -I/usr/include/opencv4
```
If you cannot get your code to compile, then just run the included `proj5.out` executable file.

Execute line:
```
./proj5.out <filename>.mp4 <num_threads> <SIMD_enable> <num_frames>
```
Arguments:<br>
`<filename>.mp4`: The name of your input file, e.g. `my_video.mp4`<br>
`<num_threads>`: Specify the number of worker threads to use. <br>
`<SIMD_enable>`: Set this to `1` if you would like to enable SIMD instructions. <br>
`<num_frames>`: Optional argument. Set this to a desired number of frames to process. Exempt this argument if you would like to process the entire video.

Here are example execution commands:
```
./proj5.out my_video.mp4 4 0 100
./proj5.out my_video.mp4 8 0 100
./proj5.out my_video.mp4 8 1 100
./proj5.out my_video.mp4 8 1
```

Once the code is up and running, follow the instructions given by the terminal prompts to enable different image processing effects. Note that invalid terminal inputs are not handled well, so please enter only `0` or `1` when prompted.

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
