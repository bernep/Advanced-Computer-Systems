# Project 5: Video Processing using Multiple Threads and SIMD Parallelization

## Overview
Video processing can be a time-consuming process, though its inherent nature makes it ripe for parallel processing. Any video can be broken down into individual frames, and thus into separate worker threads. Within each of these threads, SIMD instructions can be used to speed up image processing computations on the desired frame. As such, the aim of this project is to analyze how using multithreading and SIMD instructions as means of parallelization can significantly speed up video processing.

## Code Structure
There are five files in this project: <br>
`proj5.h`: Imports necessary libraries, defines datatypes, and establishes function prototypes. <br>
`proj5.cpp`: Reads in user inputs and video file, calls image processing functions, then outputs processed video. <br>
`gaussian_blur.cpp`: Implements two versions Gaussian blur: one using naive instructions, the other using SIMD instructions. <br>
`edge_detection.cpp`: Implements two versions of Canny edge detection: one using naive instructions, the other using OpenCV's Canny() function. <br>
`grayscale.cpp`: Implements grayscale manually using naive instructions. <br>

In this project implementation, most functions were implemented manually, but some implementations use OpenCV's implementation since OpenCV uses SIMD instructions to speed up image processing (also, edge detection is very difficult to implement manually using SIMD instructions). Here is the breakdown of what functions were implemented manually (`Y` indicates manual implementation, `N` indicates usage of OpenCV's API):

| Function                | Implemented Manually? |
|:-----------------------:|:---------------------:|
| Gaussian Blur (naive)   | Y                     |
| Gaussian Blur (SIMD)    | Y                     |
| Edge Detection (naive)  | Y                     |
| Edge Detection (SIMD)   | N                     |
| Grayscale (naive)       | Y                     |
| Grayscale (SIMD)        | N                     |

## Setup Instructions
1) OpenCV is needed to compile this program. Please follow the instructions for OpenCV setup [here](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html).
2) `ffmpeg` is not technically required, but without it, the output video will not have any audio. On Debian systems, this can be installed and then verified with the following commands:
```
sudo apt update
sudo apt install ffmpeg
ffmpeg -version
```

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
`<SIMD_enable>`: Set this to `1` if you would like to enable SIMD instructions, or `0` otherwise. <br>
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

## Experimental Results

I used the first funny video that I could find as a means of testing the video processing algorithms. The video can be found [here](https://www.youtube.com/watch?v=TnlakHr-O4w), and the video metadata is as follows:

| Property     | Value |
|:------------:|:-----:|
| FPS          | 24    |
| Frame Height | 540   |
| Frame Width  | 360   |
| Total Frames | 2818  |

### Image Processing Tests

The following images were generated to show how each effect impacts a given video frame. For this video frame, grayscale and edge detection are easy to see, but Gaussian blur is very subtle. Gaussian blur works better on videos with more detail, but the effect is there nonetheless. Also, OpenCV's edge detection is noticeably better than mine, both in quality and processing speed.

#### Control Image

<img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog.png" alt="Control" width="35%"/>

#### Naive-Processed Images (Gaussian Blur, Grayscale, Edge Detection, All Effects)

<p float="left">
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog0_gauss.png" alt="Gaussian Blur" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog0_gray.png" alt="Gaussian Blur" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog0_edge.png" alt="Edge Detection" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog0_full.png" alt="All Effects" width="35%"/>
</p>

#### SIMD-Processed Images (Gaussian Blur, Grayscale, Edge Detection, All Effects)
<p float="left">
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog1_gauss.png" alt="Gaussian Blur" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog1_gray.png" alt="Gaussian Blur" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog1_edge.png" alt="Edge Detection" width="35%"/>
  <img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/dog1_full.png" alt="All Effects" width="35%"/>
</p>

### Video Processing Tests

For the given video, 100 frames were processed with different settings. Grayscale was skipped due to its simplicity, as well as the fact that grayscaling is needed for edge detection anyway.

#### Gaussian Blur
| Number of Threads | Naive Processing Time (s) | SIMD Processing Time (s) |
|:-----------------:|:-------------------------:|:------------------------:|
| 1                 | 7.61                      | 4.39                     |
| 2                 | 4.11                      | 2.36                     |
| 4                 | 2.72                      | 1.53                     |
| 8                 | 2.29                      | 1.33                     |
<img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/gauss_graph.png" alt="Gaussian Blur Graph" width="62%"/>

#### Edge Detection
| Number of Threads | Naive Processing Time (s) | SIMD Processing Time (s) |
|:-----------------:|:-------------------------:|:------------------------:|
| 1                 | 1.13                      | 0.95                     |
| 2                 | 0.60                      | 0.50                     |
| 4                 | 0.40                      | 0.25                     |
| 8                 | 0.23                      | 0.13                     |
<img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/edge_graph.png" alt="Edge Detection Graph" width="62%"/>

#### Gaussian Blur + Edge Detection
| Number of Threads | Naive Processing Time (s) | SIMD Processing Time (s) |
|:-----------------:|:-------------------------:|:------------------------:|
| 1                 | 8.22                      | 5.04                     |
| 2                 | 4.35                      | 2.59                     |
| 4                 | 2.97                      | 1.69                     |
| 8                 | 2.45                      | 1.34                     |
<img src="https://github.com/bernep/Advanced-Computer-Systems/blob/main/Project%205/images/full_graph.png" alt="Full Graph" width="62%"/>

## Analysis

In every case, increasing the number of threads reduced processing time, and enabling SIMD instructions reduced processing time. The slowest process by far was Gaussian blur since it requries a quadruple nested loop, which I was able to speed up with SIMD instructions. That being said, OpenCV's Gaussian blur is much faster than mine since they also use SIMD instructions, though they have had far more time and expertise to optimize their implementation. Additionally, OpenCV's Canny edge detection algorithm is done much better than mine (both in quality and processing speed) since mine is a manually-implemented naive algorithm and OpenCV's is a more researched, SIMD implementation.

## Conclusion

The advantages of parallelizing video processing tasks using multithreading and SIMD instructions are very clear. The results indicate that employing these techniques can substantially reduce the processing time required for computationally intensive video processing tasks, such as Gaussian blur and edge detection. It is clear that leveraging the full potential of modern hardware through parallelization techniques can lead to significant performance improvements in video processing.
