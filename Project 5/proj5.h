/* projh.cpp*/
/* Patrick Berne, Andre Shibata */

// Example Compile Line: g++ -std=c++11 proj5.cpp gaussian_blur.cpp grayscale.cpp edge_detection.cpp -o proj5.out -lopencv_core -lopencv_videoio -lopencv_imgproc -I/usr/include/opencv4
// Example Execute Line: ./proj5.out my_video.mp4 8 1 24

// Import libraries
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <cmath>
#include <immintrin.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/intrin.hpp>

// Definitions
#define uint unsigned int

// Namespaces
using namespace std;

// Struct to hold frame and metadata information
struct FrameData {
    cv::Mat frame;
    int frame_num;
    int frame_width;
    int frame_height;
    int thread_num;
};

// Function prototypes
void gaussian_blur(FrameData* f_dat, int k_size, double sigma);
void simd_gaussian_blur(FrameData* f_dat, int k_size, double sigma);
void grayscale(FrameData* f_dat);
void simd_grayscale(FrameData* f_dat);
void edge_detection(FrameData* f_dat);
void simd_edge_detection(FrameData* f_dat);