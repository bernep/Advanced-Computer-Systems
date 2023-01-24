/* proj1.c */
/* Patrick Berne, Andre Shibata */

/* Example Compile Line: "gcc proj1.c -lzstd -I/usr/include/zstd -L/usr/lib -pthread -o proj1.out" */
/* Example Execute Line: "./proj1.out 10 input.txt"*/
/* Example Uncompress Line: "unzstd input.txt.zst" */

// Import Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>   // requires -pthread compile line
#include <zstd.h>      // requires -lzstd compile line
#include "common.h"    // contains some standard ZSTD helper functions

// Define data block size
#define DATA_BLOCK_SIZE (1 << 14) // 16kB (2^14 bytes) using bit shifting

// Define Thread Data Struct
struct ThreadPacket {
    int id;
    FILE* input_fp;
    FILE* output_fp;
    ZSTD_outBuffer zstd_data;
};

// Define global variables to keep track of threads
int NUM_THREADS = 0;
int ACTIVE_THREAD = 0;
int COMPRESSION_DONE = 0;

// Function: Compress data
static void* process_data(void* thread_packet) {
	// Grab thread packet, turn into more readable format
	struct ThreadPacket tp = *(struct ThreadPacket*)thread_packet;

	// Initialize data buffers
   	char* input_data = malloc(DATA_BLOCK_SIZE);
   	size_t output_size = ZSTD_compressBound(DATA_BLOCK_SIZE);
   	char* output_data = malloc(output_size);

	// Read data from file into an input data buffer
    int bytes_read = fread(input_data, 1, DATA_BLOCK_SIZE, tp.input_fp);

    // Initialize the ZSTD compression context
    ZSTD_CStream* cstream = ZSTD_createCStream();
    int ret = ZSTD_initCStream(cstream, 1);
    if (cstream == NULL || ZSTD_isError(ret)) {
    	fprintf(stderr, "ERROR: failed to initialize the compression context");
        return NULL;
    }

    // Initialize the ZSTD data buffers
    ZSTD_inBuffer zstd_in = { input_data, DATA_BLOCK_SIZE, 0 };
    ZSTD_outBuffer zstd_out = { output_data, output_size, 0 };

	// Compress the data
    while (zstd_in.pos < zstd_in.size) {
        ret = ZSTD_compressStream(cstream, &zstd_out, &zstd_in);
        if (ZSTD_isError(ret)) {
        	fprintf(stderr, "ERROR: failed to compress data");
            return NULL;
        }
    }

    // Finish the compression
    ret = ZSTD_endStream(cstream, &zstd_out);
    if (ZSTD_isError(ret)) {
    	fprintf(stderr, "ERROR: failed to compress data");
        return NULL;
    }

    // Wait for current thread to be active so-as to maintain data order
    while (tp.id != ACTIVE_THREAD);

    // Write compressed data block to output file
    fflush(stdout);
    if (bytes_read > 0) {
        fwrite_orDie(output_data, zstd_out.pos, tp.output_fp);
    }

    // Update global thread tracker
    ACTIVE_THREAD++;
    if (ACTIVE_THREAD >= NUM_THREADS) {
        ACTIVE_THREAD = 0;
    }

    // Additionally, check if the file compression is done
    if (bytes_read == 0) {
        COMPRESSION_DONE = 1;
    }

    // Cleanup
    free(input_data);
    free(output_data);
    ZSTD_freeCStream(cstream);

    // End threading
    return NULL;
}

/* Function: Check if string is a valid integer */
int detect_int(char* ptr) {
    char* end_ptr;
    strtol(ptr, &end_ptr, 10);
    if (*end_ptr == '\0' && atoi(ptr) >= 0) {
        return 1; // valid integer
    } else {
        return 0; // invalid integer
    }
}

/* Main Function */
int main(int argc, char *argv[]) {
	// Command-line error check. Not extremely rigorous, but should suffice.
    if (argc != 3 || detect_int(argv[1]) == 0) {
    	fprintf(stderr, "ERROR: Invalid argument(s)\n"\
    					"USAGE: proj1.out <num_threads> <input_file.txt> \n");
        return EXIT_FAILURE;
    }

	// Grab command-line arguments
	NUM_THREADS = atoi(argv[1]);
	char* input_filename = argv[2];
	char output_filename[sizeof(argv[2])+sizeof(".zst")];

   	// Set up the input and output files
	FILE* input_fp = fopen(input_filename, "rb");
	if (input_fp == NULL) {
		fprintf(stderr, "ERROR: input file does not exist\n");
		return EXIT_FAILURE;
	}
	strcpy(output_filename, input_filename);
	strcat(output_filename, ".zst");
	FILE* output_fp = fopen(output_filename, "wb");

	// Set up thread packet and pointer arrays
	struct ThreadPacket packets[NUM_THREADS];
   	pthread_t threads[NUM_THREADS];

    // Wait for file compression to finish
    while (COMPRESSION_DONE != 1) {
       	// Create several worker threads to compress and write data blocks
       	for (size_t i=0; i<NUM_THREADS; ++i) {
    		struct ThreadPacket tp;
    	    tp.id = i;
    	    tp.input_fp = input_fp;
    	    tp.output_fp = output_fp;
    	    packets[i] = tp;
        	pthread_create(threads+i, NULL, process_data, &packets[i]);
       	}

       	// Rejoin threads before ending program
       	for (size_t i=0; i<NUM_THREADS; ++i) {
       		// Rejoin thread
       		void *retval;
            pthread_join(threads[i], &retval);

            // Check for thread execution errors
            if (retval == PTHREAD_CANCELED) {
                fprintf(stderr, "ERROR: Thread #%d was cancelled\n", (int)i);
                return EXIT_FAILURE;
            }
       	}
    }

   	// Close input file
    fclose(input_fp);

    // End program
   	return EXIT_SUCCESS;
}
