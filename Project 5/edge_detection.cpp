/* edge_detection.cpp */

// Import libraries
#include "proj5.h"

/* Note: These functions work better when Gaussian blur is applied first since it reduces noise */

// Function: Canny edge detection (naive implementation)
void edge_detection(FrameData* f_dat, double low, double high, int k_size) {
    // Convert frame to grayscale (needed for edge detection to work)
    grayscale(f_dat);

    // Initialize variables
    Mat& src = f_dat->frame;
    Mat gray_src(src.rows, src.cols, CV_8UC1);
    cvtColor(src, gray_src, COLOR_BGR2GRAY);

    // Compute gradient (magnitude and angle)
    Mat grad_x, grad_y;
    Mat grad_magnitude(src.rows, src.cols, CV_32F);
    Mat grad_angle(src.rows, src.cols, CV_32F);

    Sobel(gray_src, grad_x, CV_32F, 1, 0, k_size);
    Sobel(gray_src, grad_y, CV_32F, 0, 1, k_size);

    cartToPolar(grad_x, grad_y, grad_magnitude, grad_angle, true);

    // Non-maximum suppression
    Mat nms(src.rows, src.cols, CV_8UC1, Scalar::all(0));

    for (int y = 1; y < src.rows - 1; y++) {
        for (int x = 1; x < src.cols - 1; x++) {
            float magnitude = grad_magnitude.at<float>(y, x);
            float angle = grad_angle.at<float>(y, x);
            int dx1, dy1, dx2, dy2;

            if (angle < 22.5 || angle > 157.5) {
                dx1 = 1; dy1 = 0; dx2 = -1; dy2 = 0;
            } else if (angle >= 22.5 && angle <= 67.5) {
                dx1 = 1; dy1 = 1; dx2 = -1; dy2 = -1;
            } else if (angle > 67.5 && angle < 112.5) {
                dx1 = 0; dy1 = 1; dx2 = 0; dy2 = -1;
            } else {
                dx1 = -1; dy1 = 1; dx2 = 1; dy2 = -1;
            }

            if (magnitude > grad_magnitude.at<float>(y + dy1, x + dx1) &&
                magnitude > grad_magnitude.at<float>(y + dy2, x + dx2)) {
                nms.at<uchar>(y, x) = saturate_cast<uchar>(magnitude);
            }
        }
    }

    // Hysteresis thresholding
    Mat edges(src.rows, src.cols, CV_8UC1, Scalar::all(0));

    for (int y = 1; y < src.rows - 1; y++) {
        for (int x = 1; x < src.cols - 1; x++) {
            uchar value = nms.at<uchar>(y, x);
            if (value >= high) {
                edges.at<uchar>(y, x) = 255;
            } else if (value >= low) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        if (nms.at<uchar>(y + dy, x + dx) >= high) {
                            edges.at<uchar>(y, x) = 255;
                            break;
                        }
                    }
                    if (edges.at<uchar>(y, x) == 255) {
                        break;
                    }
                }
            }
        }
    }

    // Copy the result back to the input frame
    src = edges.clone();
}

// Function: Canny edge detection (SIMD implementation)
void simd_edge_detection(FrameData* f_dat, double low, double high, int k_size) {
    // Create a new output image
    Mat edges;
    
    // Canny edge detection (OpenCV uses SIMD instructions in their implementation)
    Canny(f_dat->frame, edges, low, high, k_size);

    // Copy the output image back to the input image
    edges.copyTo(f_dat->frame);
}
