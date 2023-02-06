/* proj1.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj2.c -o proj2.out -mavx"
// Example Execute Line: "./proj2.out 10 10 -fo"
// Valid Arguments: -fo -fu -xo -xu

// Import Libraries
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Debug
int PRINT_FLAG = 1; // 0 = No Print; 1 = Print

// Function: Check if string is a valid integer
int detect_int(char* ptr) {
    char* end_ptr;
    strtol(ptr, &end_ptr, 10);
    if (*end_ptr == '\0' && atoi(ptr) >= 0) {
        return 1; // Valid integer
    } else {
        return 0; // Invalid integer
    }
}

// Function: Print matrix contents (floating-point)
void print_matrix_float(float** mat, int n, char* msg) {
    if (PRINT_FLAG == 1) { // Only print if flag is set
        printf("\n%s:\n", msg);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n ; ++j) {
                printf("%f ", mat[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

// Function: Print matrix contents (fixed-point)
void print_matrix_fixed(short int** mat, int n, char* msg) {
    if (PRINT_FLAG == 1) { // Only print if flag is set
        printf("\n%s:\n", msg);
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n ; ++j) {
                printf("%hd ", mat[i][j]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

// Function: Multiply two matrices together using intrinsic library (floating point)
void matrix_mult_float_optimized(float** mat_in1, float** mat_in2, float** mat_out, int n) {
    // ---> Below is my attempt to implement matrix multiplication. Doesn't work.
    // ---> This library is still confusing to me, so this'll take some more exploration.
    // ---> At the very least, the rest of the code is done.
    // ---> The last things we need to implement are the following SIMD x86 functions:
    //              matrix_mult_float_optimized()
    //              matrix_mult_fixed_optimized()

    // Variable setup
    __m256 a, b, c;
    float* buf = (float*)malloc(8*sizeof(float));

    // Matrix multiplication
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            c = _mm256_setzero_ps(); // Initialize a 256-bit register with zeros
            for (int k = 0; k < n; k += 8) {
                a = _mm256_loadu_ps(mat_in1[i] + k); // Load 8 float values from matrix 1
                b = _mm256_loadu_ps(mat_in2[k] + j); // Load 8 float values from matrix 2
                c = _mm256_add_ps(c, _mm256_mul_ps(a, b)); // Multiply and accumulate result
            }
            // Buffer operations
            _mm256_storeu_ps(buf, c); // Store the 256-bit result in an array of floats
            mat_out[i][j] = 0;
            for (int k = 0; k < 8; k++) {
                mat_out[i][j] += buf[k]; // Store buffer values int matrix
            }
        }
    }

    // Memory cleanup
    free(buf);
}

// Function: Multiply two matrices together using intrinsic library (fixed point)
void matrix_mult_fixed_optimized(short int** mat_in1, short int** mat_in2,\
                                 short int** mat_out, int n) {
    // TBD
}

// Function: Multiple two matrices together using standard C operations (floating point)
void matrix_mult_float_unoptimized(float** mat_in1, float** mat_in2,\
                                   float** mat_out, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat_in1[i][k] * mat_in2[k][j];
            }
            mat_out[i][j] = sum; 
        }
    }
}

// Function: Multiple two matrices together using standard C operations (fixed point)
void matrix_mult_fixed_unoptimized(short int** mat_in1, short int** mat_in2,\
                                   short int** mat_out, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat_in1[i][k] * mat_in2[k][j];
            }
            mat_out[i][j] = sum;
        }
    }
}

// Function: Main
int main(int argc, char *argv[])
{
    // Command-line error check.
    if (argc != 4 || detect_int(argv[1]) == 0 || detect_int(argv[2]) == 0\
        || (strcmp(argv[3],"-fo") != 0 && strcmp(argv[3],"-fu") != 0 &&
            strcmp(argv[3],"-xo") != 0 && strcmp(argv[3],"-xu") != 0)) {

        fprintf(stderr, "ERROR:\tInvalid argument(s)\n"\
                        "USAGE:\tproj1.out <n> <seed> <-arg> \n"\
                        "ARGS:\t-fo --> 4-byte floating point, optimized\n"\
                             "\t-fu --> 4-byte floating point, unoptimized\n"\
                             "\t-xo --> 2-byte fixed point, optimized\n"\
                             "\t-xu --> 2-byte fixed point, unoptimized\n");

        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    int n = atoi(argv[1]);
    int seed = atoi(argv[2]);
    char* arg = argv[3];

    // Seed randomizer
    srand(seed);

    // Matrix operations (using 4-byte floating-point)
    if (strcmp(arg,"-fo") == 0 || strcmp(arg,"-fu") == 0) {
        // Create n-by-n matrices by allocating memory appropriately
        float** mat_in = (float**)malloc(n*sizeof(float*)); // Row allocation
        float** mat_out = (float**)malloc(n*sizeof(float*)); // Row allocation
        for (int i = 0; i < n; i++) {
            mat_in[i] = (float*)malloc(n*sizeof(float)); // Col allocation
            mat_out[i] = (float*)malloc(n*sizeof(float)); // Col allocation
        }

        // Populate matrix with random numbers between 0 and 1
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n ; ++j) {
                mat_in[i][j] = (float)rand()/RAND_MAX;
            }
        }

        // Matrix multiplication (select between optimized and unoptimized)
        print_matrix_float(mat_in, n, "Floating-Point Input"); // Print (if enabled)
        if (strcmp(arg,"-fo") == 0) {
            matrix_mult_float_optimized(mat_in, mat_in, mat_out, n);
            print_matrix_float(mat_out, n, "Floating-Point Optimized Output"); // Print (if enabled)
        } else if (strcmp(arg,"-fu") == 0) {
            matrix_mult_float_unoptimized(mat_in, mat_in, mat_out, n);
            print_matrix_float(mat_out, n, "Floating-Point Unoptimized Output"); // Print (if enabled)
        }

        // Memory cleanup
        for (int i = 0; i < n; i++) {
            free(mat_in[i]);
            free(mat_out[i]);
        }
        free(mat_in);
        free(mat_out);
    }

    // Matrix operations (using 2-byte fixed-point)
    if (strcmp(arg,"-xo") == 0 || strcmp(arg,"-xu") == 0) {
        // Create n-by-n matrices by allocating memory appropriately
        short int** mat_in = (short int**)malloc(n*sizeof(short int*)); // Row allocation
        short int** mat_out = (short int**)malloc(n*sizeof(short int*)); // Row allocation
        for (int i = 0; i < n; i++) {
            mat_in[i] = (short int*)malloc(n*sizeof(short int)); // Col allocation
            mat_out[i] = (short int*)malloc(n*sizeof(short int)); // Col allocation
        }

        // Populate matrix with random numbers between 0 and 100
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n ; ++j) {
                mat_in[i][j] = (short int)(rand() % 100);
            }
        }

        // Matrix multiplication (select between optimized and unoptimized)
        print_matrix_fixed(mat_in, n, "Fixed-Point Input"); // Print (if enabled)
        if (strcmp(arg,"-xo") == 0) {
            matrix_mult_fixed_optimized(mat_in, mat_in, mat_out, n);
            print_matrix_fixed(mat_out, n, "Fixed-Point Optimized Output"); // Print (if enabled)
        } else if (strcmp(arg,"-xu") == 0) {
            matrix_mult_fixed_unoptimized(mat_in, mat_in, mat_out, n);
            print_matrix_fixed(mat_out, n, "Fixed-Point Unoptimized Output"); // Print (if enabled)
        }

        // Memory cleanup
        for (int i = 0; i < n; i++) {
            free(mat_in[i]);
            free(mat_out[i]);
        }
        free(mat_in);
        free(mat_out);
    }

    return EXIT_SUCCESS;
}
