/* proj4.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj2.c -o proj2.out -mavx2 -mavx512bw -mavx512vl"
// Example Execute Line: "./proj4.out col.txt"

// Import libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <immintrin.h>
#include <pthread.h>
#include <time.h>

// Define struct for thread data
struct ThreadPacket {
    int row_num;
    char* data;
    FILE* output_file;
};

// Global variables
int num_cols = 0;
pthread_mutex_t file_mut = PTHREAD_MUTEX_INITIALIZER;

// Function: Dictionary encoder as a worker thread
void *dictionary_encode(void* thread_data) {
    // Grab thread data, turn into more readable format
    struct ThreadPacket* tp = thread_data;
    
    // TBD
    int key = 0; // TBD

    // Write dictionary and encoded column to output file with mutex locking
    pthread_mutex_lock(&file_mut);
    fprintf(tp->output_file, "%d\t%s\n", key, tp->data);
    fflush(tp->output_file);
    pthread_mutex_unlock(&file_mut);

    // End thread
    pthread_exit(NULL);
}


// Function: Main
int main(int argc, char* argv[])
{
    // Begin execution timer
    clock_t begin = clock();

    // Command-line error check.
    if (argc < 2) {
        fprintf(stderr, "ERROR:\tInvalid argument(s)\n"\
                        "USAGE:\tproj4.out <input_file> \n");
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    char* input_filename = argv[1];
    
    // Check if input files exist, and create pointers to them
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "ERROR: Input file missing\n");
        return EXIT_FAILURE;
    }
    
    // Create output file and its pointer
    char output_filename[1024];
    strcpy(output_filename, "dict_");
    strcat(output_filename, input_filename);
    FILE* output_file = fopen(output_filename, "w");
    
    // Count the number of columns in the input file
    char line[1024];
    while (fgets(line, sizeof(line), input_file)) {
        num_cols++;
    }
    rewind(input_file);
    
    // Create array of thread IDs for each worker thread
    pthread_t thread_ids[num_cols];

    // Create array of thread data for each column
    struct ThreadPacket* threads = malloc(num_cols * sizeof(struct ThreadPacket));

    // Read in each column of data from the input file and spawn a thread for it
    int row_num = 1;
    while (fgets(line, sizeof(line), input_file)) {
        // Remove trailing newline from line
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
    
        // Check if line is empty
        if (strlen(line) == 0) {
            continue;
        }
        
        // Copy column data into thread data structure
        threads[row_num - 1].row_num = row_num;
        threads[row_num - 1].data = strdup(line);
        threads[row_num - 1].output_file = output_file;
        
        // Spawn thread to encode this column of data
        pthread_create(&thread_ids[row_num - 1], NULL, dictionary_encode, (void*)&threads[row_num - 1]);
        
        row_num++;
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_cols; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Close files and clean up
    fclose(input_file);
    fclose(output_file);
    free(threads);

    // Print execution time
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nELAPSED TIME:\n--->%f seconds\n", time_spent);

    return EXIT_SUCCESS;
}
