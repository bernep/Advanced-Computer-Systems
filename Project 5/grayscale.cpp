/* grayscale.cpp */

// Import libraries
#include "proj5.h"
#include <emmintrin.h>

// Function: Grayscale
void grayscale(FrameData* f_dat) {
    // Initialize variables
    Mat& src = f_dat->frame;

    // Convert frame to grayscale
    for(int i=0;i<src.rows;i++){
        for(int j=0;j<src.cols;j++){
            Vec3b & color = src.at<Vec3b>(i,j);

            // Average each color channel
            int sum = color[0]+color[1]+color[2];
            sum /= 3;
            color[0] = sum;
            color[1] = sum;
            color[2] = sum;
            src.at<Vec3b>(Point(i,j)) = color;
        }
    }
}
