/* proj1.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj2.c -o proj2.out -mavx"
// Example Execute Line: "./proj2.out 10 10 -f"

// Import Libraries
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Function: Check if string is a valid integer
int detect_int(char* ptr) {
    char* end_ptr;
    strtol(ptr, &end_ptr, 10);
    if (*end_ptr == '\0' && atoi(ptr) >= 0) {
        return 1; // valid integer
    } else {
        return 0; // invalid integer
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

    // --> At the moment, these arguments don't do anything. We can implement that later.
    // --> Let's make one version of the code work first.

    // Grab command-line arguments
    int n = atoi(argv[1]);
    int seed = atoi(argv[2]);

    // Create n-by-n matrix by allocating memory appropriately
    float** mat = (float**)malloc(n*sizeof(float*)); // row allocation
    for (int i = 0; i < n; i++) {
        mat[i] = (float*)malloc(n*sizeof(float)); // col allocation
    }

    // Seed randomizer
    srand(seed);

    // Populate matrix with random numbers between 0 and 1
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n ; ++j) {
            mat[i][j] = (float)rand()/RAND_MAX;
        }
    }

    // Print matrix (for debugging)
    /*for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n ; ++j) {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }*/

    // Matrix multiplication time (this is where the fun begins)
    /*
    -----> What do we do now????
    */

    // Memory Cleanup
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);

    return EXIT_SUCCESS;
}
