/* gaussian_blur.cpp */

// Import libraries
#include "proj5.h"

// Function: Gaussian blur (naive implementation)
void gaussian_blur(FrameData* f_dat, int k_size, double sigma) {
    // Initialize variables
    Mat& src = f_dat->frame;
    Mat dst(src.rows, src.cols, src.type());

    // Generate kernel
    Mat kernelX = getGaussianKernel(k_size, sigma);
    Mat kernelY = getGaussianKernel(k_size, sigma);
    Mat kernel = kernelX * kernelY.t();

    // Apply blur
    for (int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {
            Vec3d p = Vec3d(0, 0, 0);
            for(int k = 0; k < kernel.rows; k++) {
                for(int l = 0; l < kernel.cols; l++) {
                    int y = i + k - kernel.rows / 2;
                    int x = j + l - kernel.cols / 2;
                    if (y >= 0 && y < src.rows && x >= 0 && x < src.cols) {
                        p += src.at<Vec3b>(y, x) * kernel.at<double>(k, l);
                    }
                }
            }

            dst.at<Vec3b>(i, j) = Vec3b(p);
        }
    }

    // Replace the input frame with the blurred frame
    src = dst;
}

// Function: Gaussian blur (SIMD implementation)
void simd_gaussian_blur(FrameData* f_dat, int k_size, double sigma) {
    // Initialize variables
    Mat& src = f_dat->frame;
    Mat dst(src.rows, src.cols, src.type());

    // Generate kernel
    Mat kernelX = getGaussianKernel(k_size, sigma);
    Mat kernelY = getGaussianKernel(k_size, sigma);
    Mat kernel = kernelX * kernelY.t();

    // Apply blur
    for (int i = 0; i < src.rows; i++) {
        for(int j = 0; j < src.cols; j++) {
            __m128d p0 = _mm_setzero_pd();
            __m128d p1 = _mm_setzero_pd();

            for(int k = 0; k < kernel.rows; k++) {
                for(int l = 0; l < kernel.cols; l++) {
                    int y = i + k - kernel.rows / 2;
                    int x = j + l - kernel.cols / 2;
                    if (y >= 0 && y < src.rows && x >= 0 && x < src.cols) {
                        Vec3b vec = src.at<Vec3b>(y, x);
                        double val = kernel.at<double>(k, l);
                        __m128d m_val = _mm_set1_pd(val);
                        __m128i m_vec = _mm_set_epi32(0, vec[2], vec[1], vec[0]);
                        __m128d m_vec_lo = _mm_cvtepi32_pd(m_vec);
                        __m128d m_vec_hi = _mm_cvtepi32_pd(_mm_unpackhi_epi64(m_vec, m_vec));
                        p0 = _mm_add_pd(p0, _mm_mul_pd(m_vec_lo, m_val));
                        p1 = _mm_add_pd(p1, _mm_mul_pd(m_vec_hi, m_val));
                    }
                }
            }

            int b = (int)(_mm_cvtsd_f64(p0) + 0.5);
            int g = (int)(_mm_cvtsd_f64(_mm_unpackhi_pd(p0, p0)) + 0.5);
            int r = (int)(_mm_cvtsd_f64(p1) + 0.5);
            dst.at<Vec3b>(i, j) = Vec3b(b, g, r);
        }
    }

    // Replace the input frame with the blurred frame
    src = dst;
}
