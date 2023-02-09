/* proj1.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj2.c -o proj2.out -mavx2 -mavx512bw -mavx512vl"
// Example Execute Line: "./proj2.out 10 10 -fo -pm"
// Valid Arguments: -fo -fu -xo -xu

// Import libraries
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Print flags (these are set by command-line arguments)
int PROGRESS_PRINT = 0; // Initially disabled
int MATRIX_PRINT = 0; // Initially disabled

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
    printf("\n%s:\n", msg);
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n ; ++j) {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function: Print matrix contents (fixed-point)
void print_matrix_fixed(short** mat, int n, char* msg) {
    printf("\n%s:\n", msg);
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n ; ++j) {
            printf("%d\t", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function: Multiply two matrices together using intrinsic library (floating-point)
void matrix_mult_float_optimized(float** mat_in1, float** mat_in2, float** mat_out, int n) {
    // Variable setup
    __m256 a, b, res, sum;
    float sum_buf[8] __attribute__((aligned(32)));

    // Transpose second matrix into temporary matrix (this is necessary for faster value loading)
    float** mat_trans = (float**)_mm_malloc(n*sizeof(float*), 32);
    for (int i = 0; i < n; i++) {
        mat_trans[i] = (float*)_mm_malloc(n*sizeof(float), 32);
        for (int j = 0; j < n; j++) {
            mat_trans[i][j] = mat_in1[j][i];
        }
    }

    // Matrix multiplication
    if (PROGRESS_PRINT) printf("PROGRESS:\n");
    for (int i = 0; i < n; i++) {
        if (i % 100 == 0) {
            if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", i, n);
        }
        for (int j = 0; j < n; j++) {
            res = _mm256_setzero_ps(); // Initialize a 256-bit register with zeros
            for (int k = 0; k < n; k += 8) {
                // Multiply and accumulate current 32-byte blocks
                a = _mm256_load_ps(&mat_trans[i][k]); // Load (up to) 8 float values from matrix 1
                b = _mm256_load_ps(&mat_in2[j][k]); // Load (up to) 8 float values from matrix 2
                res = _mm256_add_ps(res, _mm256_mul_ps(a, b)); // Multiply and accumulate result

                // Sum each buffer result, then store in output matrix
                sum = _mm256_hadd_ps(res, res);
                sum = _mm256_hadd_ps(sum, sum);
                _mm256_storeu_ps(sum_buf, sum);
                mat_out[j][i] = sum_buf[0] + sum_buf[4]; // Store the summed values into output
            }
        }
    }
    if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", n, n);

    // Memory cleanup
    for (int i = 0; i < n; i++) {
        _mm_free(mat_trans[i]);
    }
    _mm_free(mat_trans);
}

// Function: Multiply two matrices together using intrinsic library (short integers)
void matrix_mult_fixed_optimized(short** mat_in1, short** mat_in2, short** mat_out, int n) {
    // Variable setup
    __m128i a, b, res, sum;
    short sum_buf[8] __attribute__((aligned(16)));

    // Transpose second matrix into temporary matrix (this is necessary for faster value loading)
    short** mat_trans = (short**)_mm_malloc(n*sizeof(short*), 16);
    for (int i = 0; i < n; i++) {
        mat_trans[i] = (short*)_mm_malloc(n*sizeof(short), 16);
        for (int j = 0; j < n; j++) {
            mat_trans[i][j] = mat_in1[j][i];
        }
    }

    // Matrix multiplication
    if (PROGRESS_PRINT) printf("PROGRESS:\n");
    for (int i = 0; i < n; i++) {
        if (i % 100 == 0) {
            if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", i, n);
        }
        for (int j = 0; j < n; j++) {
            res = _mm_setzero_si128(); // Initialize a 128-bit register with zeros
            for (int k = 0; k < n; k += 8) {
                // Multiply and accumulate current 16-byte blocks
                a = _mm_loadu_epi16(&mat_trans[i][k]); // Load (up to) 8 short values from matrix 1
                b = _mm_loadu_epi16(&mat_in2[j][k]); // Load (up to) 8 short values from matrix 2
                res = _mm_add_epi16(res, _mm_mullo_epi16(a, b)); // Multiply and accumulate result

                // Sum each buffer result, then store in output matrix
                sum = _mm_hadd_epi16(res, res);
                sum = _mm_hadd_epi16(sum, sum);
                sum = _mm_hadd_epi16(sum, sum);
                _mm_storeu_epi16(sum_buf, sum);
                mat_out[j][i] = sum_buf[0]; // Store the first (summed) value into output matrix
            }
        }
    }
    if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", n, n);

    // Memory cleanup
    for (int i = 0; i < n; i++) {
        _mm_free(mat_trans[i]);
    }
    _mm_free(mat_trans);
}

// Function: Multiple two matrices together using standard C operations (floating-point)
void matrix_mult_float_unoptimized(float** mat_in1, float** mat_in2,\
                                   float** mat_out, int n) {
    if (PROGRESS_PRINT) printf("PROGRESS:\n");
    for (int i = 0; i < n; i++) {
        if ((i % 100) == 0) {
            if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", i, n);
        }
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat_in1[i][k] * mat_in2[k][j];
            }
            mat_out[i][j] = sum; 
        }
    }
    if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", n, n);
}

// Function: Multiple two matrices together using standard C operations (fixed-point)
void matrix_mult_fixed_unoptimized(short** mat_in1, short** mat_in2,\
                                   short** mat_out, int n) {
    if (PROGRESS_PRINT) printf("PROGRESS:\n");
    for (int i = 0; i < n; i++) {
        if ((i % 100) == 0) {
            if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", i, n);
        }
        for (int j = 0; j < n; j++) {
            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += mat_in1[i][k] * mat_in2[k][j];
            }
            mat_out[i][j] = sum;
        }
    }
    if (PROGRESS_PRINT) printf("--->%d/%d rows computed\n", n, n);
}

// Function: Main
int main(int argc, char *argv[])
{
    // Command-line error check.
    if (argc < 4 || detect_int(argv[1]) == 0 || detect_int(argv[2]) == 0\
        || (strcmp(argv[3],"-fo") != 0 && strcmp(argv[3],"-fu") != 0 &&
            strcmp(argv[3],"-xo") != 0 && strcmp(argv[3],"-xu") != 0)) {

        fprintf(stderr, "ERROR:\tInvalid argument(s)\n"\
                        "USAGE:\tproj1.out <n> <seed> <-arg1> <-arg2> \n"\
                        "ARG1:\t-fo --> 4-byte floating-point, optimized\n"\
                             "\t-fu --> 4-byte floating-point, unoptimized\n"\
                             "\t-xo --> 2-byte fixed-point, optimized\n"\
                             "\t-xu --> 2-byte fixed-point, unoptimized\n"\
                        "ARG2:\t-p --> print progress\n"\
                             "\t-m --> print matrices\n"\
                             "\t-pm --> print both progress and matrices\n"\
                        "NOTE:\tARG2 is optional.\n");

        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    int n = atoi(argv[1]);
    int seed = atoi(argv[2]);
    char* arg1 = argv[3];
    char* arg2;
    if (argc > 4) {
        arg2 = argv[4];
    }

    // Seed randomizer
    srand(seed);

    // Set print flags based on arguments
    if (argc > 4 && (strcmp(arg2,"-p") == 0 || strcmp(arg2,"-pm") == 0 || strcmp(arg2,"-mp") == 0)) {
        PROGRESS_PRINT = 1; // Print enabled
    }
    if (argc > 4 && (strcmp(arg2,"-m") == 0 || strcmp(arg2,"-pm") == 0 || strcmp(arg2,"-mp") == 0)) {
        MATRIX_PRINT = 1; // Print enabled
    }

    // Matrix operations (using 4-byte floating-point)
    if (strcmp(arg1,"-fo") == 0 || strcmp(arg1,"-fu") == 0) {
        // Optimized version: uses SIMD instructions
        if (strcmp(arg1,"-fo") == 0) {
            // Create n-by-n matrices by allocating memory appropriately
            float** mat_in = (float**)_mm_malloc(n*sizeof(float*), 32);
            float** mat_out = (float**)_mm_malloc(n*sizeof(float*), 32);
            for (int i = 0; i < n; i++) {
                mat_in[i] = (float*)_mm_malloc(n*sizeof(float), 32);
                mat_out[i] = (float*)_mm_malloc(n*sizeof(float), 32);
            }

            // Populate matrix with random numbers between 0 and 1
            for(int i = 0; i < n; ++i) {
                for(int j = 0; j < n ; ++j) {
                    mat_in[i][j] = (float)rand()/RAND_MAX;
                }
            }

            // Print input matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_float(mat_in, n, "Floating-Point Input");
            }

            // Matrix multiplication
            matrix_mult_float_optimized(mat_in, mat_in, mat_out, n);

            // Print output matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_float(mat_out, n, "Floating-Point Optimized Output");
            }

            // Memory cleanup
            for (int i = 0; i < n; i++) {
                _mm_free(mat_in[i]);
                _mm_free(mat_out[i]);
            }
            _mm_free(mat_in);
            _mm_free(mat_out);
        } 

        // Unoptimized version: uses standard C instructions
        else if (strcmp(arg1,"-fu") == 0) {
            // Create n-by-n matrices by allocating memory appropriately
            float** mat_in = (float**)malloc(n*sizeof(float*));
            float** mat_out = (float**)malloc(n*sizeof(float*));
            for (int i = 0; i < n; i++) {
                mat_in[i] = (float*)malloc(n*sizeof(float));
                mat_out[i] = (float*)malloc(n*sizeof(float));
            }

            // Populate matrix with random numbers between 0 and 1
            for(int i = 0; i < n; ++i) {
                for(int j = 0; j < n ; ++j) {
                    mat_in[i][j] = (float)rand()/RAND_MAX;
                }
            }

            // Print input matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_float(mat_in, n, "Floating-Point Input");
            }

            // Matrix multiplication
            matrix_mult_float_unoptimized(mat_in, mat_in, mat_out, n);

            // Print output matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_float(mat_out, n, "Floating-Point Unoptimized Output");
            }

            // Memory cleanup
            for (int i = 0; i < n; i++) {
                free(mat_in[i]);
                free(mat_out[i]);
            }
            free(mat_in);
            free(mat_out);
        }
    }

    // Matrix operations (using 2-byte fixed-point)
    if (strcmp(arg1,"-xo") == 0 || strcmp(arg1,"-xu") == 0) {
        // Optimized version: uses SIMD instructions
        if (strcmp(arg1,"-xo") == 0) {
            // Create n-by-n matrices by allocating memory appropriately
            short** mat_in = (short**)_mm_malloc(n*sizeof(short*), 16);
            short** mat_out = (short**)_mm_malloc(n*sizeof(short*), 16);
            for (int i = 0; i < n; i++) {
                mat_in[i] = (short*)_mm_malloc(n*sizeof(short), 16);
                mat_out[i] = (short*)_mm_malloc(n*sizeof(short), 16);
            }

            // Populate matrix with random numbers between 0 and 10
            for(int i = 0; i < n; ++i) {
                for(int j = 0; j < n ; ++j) {
                    mat_in[i][j] = (short)rand() % 10;
                }
            }

            // Print input matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_fixed(mat_in, n, "Fixed-Point Input");
            }

            // Matrix multiplication
            matrix_mult_fixed_optimized(mat_in, mat_in, mat_out, n);

            // Print output matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_fixed(mat_out, n, "Fixed-Point Optimized Output");
            }

            // Memory cleanup
            for (int i = 0; i < n; i++) {
                _mm_free(mat_in[i]);
                _mm_free(mat_out[i]);
            }
            _mm_free(mat_in);
            _mm_free(mat_out);
        } 

        // Unoptimized version: uses standard C instructions
        else if (strcmp(arg1,"-xu") == 0) {
            // Create n-by-n matrices by allocating memory appropriately
            short** mat_in = (short**)malloc(n*sizeof(short*));
            short** mat_out = (short**)malloc(n*sizeof(short*));
            for (int i = 0; i < n; i++) {
                mat_in[i] = (short*)malloc(n*sizeof(short));
                mat_out[i] = (short*)malloc(n*sizeof(short));
            }

            // Populate matrix with random numbers between 0 and 10
            for(int i = 0; i < n; ++i) {
                for(int j = 0; j < n ; ++j) {
                    mat_in[i][j] = (short)rand() % 10;
                }
            }

            // Print input matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_fixed(mat_in, n, "Fixed-Point Input");
            }

            // Matrix multiplication
            matrix_mult_fixed_unoptimized(mat_in, mat_in, mat_out, n);

            // Print output matrix (if enabled)
            if (MATRIX_PRINT == 1) {
                print_matrix_fixed(mat_out, n, "Fixed-Point Unoptimized Output");
            }

            // Memory cleanup
            for (int i = 0; i < n; i++) {
                free(mat_in[i]);
                free(mat_out[i]);
            }
            free(mat_in);
            free(mat_out);
        }
    }

    return EXIT_SUCCESS;
}
