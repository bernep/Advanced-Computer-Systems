/* grayscale.cpp */

// Import libraries
#include "proj5.h"

// Function: Grayscale (naive implementation)
void grayscale(FrameData* f_dat) {
    // Do stuff to f_dat->frame

    cv::Mat& src = f_dat->frame;
    for(int i=0;i<src.rows;i++){
        for(int j=0;j<src.cols;j++){

            Vec3b & color = src.at<Vec3b>(i,j);

            int sum = color[0]+color[1]+color[2];
            int sum /= 3;
            color[0] = sum;
            color[1] = sum;
            color[2] = sum;
            
            src.at<Vec3b>(Point(i,j)) = color;
        }
    }


}

// Function: Grayscale (SIMD implementation)
void simd_grayscale(FrameData* f_dat) {
    // Do stuff to f_dat->frame
}