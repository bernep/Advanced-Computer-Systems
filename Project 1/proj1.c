/* proj1.c */
/* Patrick Berne, Andre Shibata */

/* Example Compile Line: "gcc -Wall proj1.c -pthread -o proj1.out" */
/* Example Execute Line: "./proj1.out 50 input.txt output.txt"*/

/* Import Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

/* Global Variables */
#define DATA_BLOCK_SIZE 1 // --> Adjust this so that it's 16kB

/* Define Thread Data Struct */
struct ThreadPacket {
    int id;
    int* data;
};

/* Compress Data */
static void* compress_data(void* thread_packet) {
	/* Grab data, turn into more readable format */
	struct ThreadPacket tp = *(struct ThreadPacket*)thread_packet;

	/* Compress data */
	// TBD. Basically, you'll want to replace the 16kB "data" in the struct with 
	//      a compressed version of the data, or you could create an entirely
	//      new part of the struct that holds the compressed data. Up to you.
	//      I *think* the pthreads should output the data in order, so once
	//      the data gets compressed here, it should all output in the correct
	//      order, assuming you grab the data from their respective packets.

    return NULL;
}

/* Check if String is a Valid, Non-Negative Integer */
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
	/* Command-line error check. Not extremely rigorous, but should suffice. */
    if (argc != 4 || detect_int(argv[1]) == 0) {
    	fprintf(stderr, "ERROR: Invalid argument(s)\n"\
    					"USAGE: proj1.out <NUM_THREADS> <input_file.txt> <output_file.txt>\n");
        return EXIT_FAILURE;
    }

	/* Grab command line arguments */
	int NUM_THREADS = atoi(argv[1]);
	char* input_filename = argv[2];
	char* output_filename = argv[3];

	/* Set up thread packet and pointer arrays */
	struct ThreadPacket packets[NUM_THREADS];
   	pthread_t threads[NUM_THREADS];

   	/* Set up files */
	FILE* input_fp = fopen(input_filename, "r");
	if (input_fp == NULL) {;
		fprintf(stderr, "Input Argument Error: input file does not exist\n");
		return EXIT_FAILURE;
	}
	FILE* output_fp = fopen(output_filename, "w");

   	/* Create several worker threads to compress different data blocks */
   	for (size_t i=0; i<NUM_THREADS; ++i) {
   		/* Define data block */
    	int data_block[DATA_BLOCK_SIZE] = {i};

   		/* Initialize data packet for this specific thread */
		struct ThreadPacket tp;
	    tp.id = i;
	    tp.data = data_block;
	    packets[i] = tp;
    	pthread_create(threads+i, NULL, compress_data, &packets[i]);
   	}

   	/* Rejoin threads, i.e. receive compressed data (in order) */
   	for (size_t i=0; i<NUM_THREADS; ++i) {
   		// Rejoin thread
   		void *retval;
        pthread_join(threads[i], &retval);

        // Check for thread execution errors
        if (retval == PTHREAD_CANCELED) {
            fprintf(stderr, "ERROR: Thread #%d was cancelled\n", (int)i);
        }
        else {
        	char* file_str = calloc(1, 1000);
        	fscanf(input_fp, "%s", file_str); // --> You'll have to replace this with compressed data blocks
        									  //     since we don't want to actually directly copy data from
        									  //     the input file.
        	fputs(file_str, output_fp); // write data to file
        	free(file_str); // free memory
        }
   	}

   	// --> Make sure you run valgrind (or something similar) and check for memory errors.
  	//     If we have any, we need to go back and add some free() statements.

   	return EXIT_SUCCESS;
}
