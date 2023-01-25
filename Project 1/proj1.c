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
    char* input_data;
    char* output_data;
    size_t zstd_pos;
};

// Function: Compress data
static void* compress_data(void* thread_packet) {
    // Grab thread packet, turn into more readable format
    struct ThreadPacket* tp = (struct ThreadPacket*)thread_packet;

    // Set up output data bugger
    size_t output_size = ZSTD_compressBound(DATA_BLOCK_SIZE);
    tp->output_data = malloc(output_size);

    // Initialize the ZSTD compression context
    ZSTD_CStream* cstream = ZSTD_createCStream();
    int ret = ZSTD_initCStream(cstream, 1);
    if (cstream == NULL || ZSTD_isError(ret)) {
        fprintf(stderr, "ERROR: failed to initialize the compression context");
        return NULL;
    }

    // Initialize the ZSTD data buffers
    ZSTD_inBuffer zstd_in = { tp->input_data, DATA_BLOCK_SIZE, 0 };
    ZSTD_outBuffer zstd_out = { tp->output_data, output_size, 0 };
    int const lastChunk = (zstd_in.pos < zstd_in.size);
    ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
        
    // Compress the data
    while (zstd_in.pos < zstd_in.size) {
        ret = ZSTD_compressStream2(cstream, &zstd_out, &zstd_in, mode);
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

    // Finalize thread data before sending back to main function
    tp->zstd_pos = zstd_out.pos;

    // Cleanup
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
    if (argc != 3 || detect_int(argv[1]) == 0 || atoi(argv[1]) < 1) {
        fprintf(stderr, "ERROR: Invalid argument(s)\n"\
                        "USAGE: proj1.out <num_threads> <input_file.txt> \n");
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    int num_threads = atoi(argv[1]);
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
    pthread_t threads[num_threads];
    struct ThreadPacket packets[num_threads];

    // Set flags to determine thread execution order
    int threading_active = 1;
    int processing_active = 1;

    // Create several worker threads to compress and write data blocks
    for (size_t i=0; i<num_threads; ++i) {
        // Set up data packet structure
        struct ThreadPacket tp;
        tp.id = i;
        tp.input_data = malloc(DATA_BLOCK_SIZE);

        // Read data into thread packet input buffer
        int bytes_read = fread(tp.input_data, 1, DATA_BLOCK_SIZE, input_fp);

        // If data was read, keep track of this packet and begin threading
        if (bytes_read > 0) {
            packets[i] = tp;
            pthread_create(threads+i, NULL, compress_data, (void*)(&packets[i]));
        } 
        // Otherwise, if no more data is read, finish executing current threads and write to output file
        else {
            // Finish executing threads
            for (size_t j=0; j<i; j++) {
                // Rejoin thread
                pthread_join(threads[j], NULL);

                // Write data to output file
                fwrite_orDie(packets[j].output_data, packets[j].zstd_pos, output_fp);
                fflush(stdout);
            }

            // Adjust number of threads
            num_threads = i;

            // Adjust tracker flags
            threading_active = 0;
            processing_active = 0;

            // Cleanup
            free(tp.input_data);

            // End Loop
            break;
        }
    }

    // Rerun threads until input file has been fully read
    int last_thread_id = 0;
    while (threading_active == 1) {
        for (size_t i=0; i<num_threads; i++) {
            // Rejoin thread
            pthread_join(threads[i], NULL);

            // Write data to output file
            fwrite_orDie(packets[i].output_data, packets[i].zstd_pos, output_fp);
            fflush(stdout);

            // Read data from input file
            int bytes_read = fread(packets[i].input_data, 1, DATA_BLOCK_SIZE, input_fp);

            // Rerun the thread if more data was read, otherwise end the loop
            if (bytes_read > 0) {
                // The input data buffer neads to be adjusted for the last data chunk
                if (bytes_read < DATA_BLOCK_SIZE) {
                    // Do some memory transfers
                    char* temp_buffer = malloc(bytes_read);
                    memcpy(temp_buffer, packets[i].input_data, bytes_read);
                    free(packets[i].input_data);
                    packets[i].input_data = malloc(bytes_read);
                    memcpy(packets[i].input_data, temp_buffer, bytes_read);
                    free(temp_buffer);
                }
                // Rerun thread
                pthread_create(threads+i, NULL, compress_data, &packets[i]);
            } else {
                // Adjust tracker variables
                last_thread_id = i;
                threading_active = 0;

                // End Loop
                break;
            }
        }
    }

    // Finish executing remaining threads
    if (processing_active == 1) {
        // Initial thread index check
        int j = last_thread_id+1;
        if (j >= num_threads) {
            j = 0;
        }
        // Execute threads
        for (size_t i=0; i<num_threads-1; i++) {
            // Rejoin thread
            pthread_join(threads[j], NULL); 

            // Write data to output file
            fwrite_orDie(packets[j].output_data, packets[j].zstd_pos, output_fp);
            fflush(stdout);

            // Adjust thread index
            j++;
            if (j >= num_threads) {
                j = 0;
            }
        }
        // Toggle tracker flag
        processing_active = 0;
    }

    // Close the files
    fclose(input_fp);
    fclose(output_fp);

    // Memory Cleanup
    for (size_t i=0; i<num_threads; i++) {
        free(packets[i].input_data);
        free(packets[i].output_data);
    }

    // End program
    return EXIT_SUCCESS;
}
