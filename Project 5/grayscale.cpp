/* grayscale.cpp */

// Import libraries
#include "proj5.h"

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
// Function: Grayscale (SIMD implementation)
void simd_grayscale(FrameData* f_dat) {
    
    __m128i a, b, c, res ,sum;
    cv::Mat& src = f_dat->frame;
    for(int i=0;i<src.rows;i++){
        for(int j=0;j<src.cols;j++){
            Vec3b & color = src.at<Vec3b>(i,j);
            a = _mm_loadu_epi16(&color[0]);
            b = _mm_loadu_epi16(&color[1]);
            c = _mm_loadu_epi16(&color[2]);
            sum = _mm_hadd_epi16(a, b);
            sum = _mm_hadd_epi16(sum, c);
            short sum_buf[8] __attribute__((aligned(16)));
            _mm_storeu_epi16(sum_buf, sum);
            short sum = sum_buf[0];
            sum /= 3;
            color[0] = sum;
            color[1] = sum;
            color[2] = sum;
            src.at<Vec3b>(Point(i,j)) = color;
        }
    }

}
