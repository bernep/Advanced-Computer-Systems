/* proj4.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj4.c bin_tree.c -o proj4.out -mavx2"
// Example Execute Line: "./proj4.out col.txt 10"

// Import libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <immintrin.h>
#include <pthread.h>
#include <time.h>
#include "bin_tree.h"

// Define struct for thread data
struct ThreadPacket {
    ushort key;
    char* data;
};

// Global variables
struct TreeNode* TREE_ROOT = NULL;
ushort ENCODED_VALUE = 1;

// Function: Dictionary encoder as a worker thread; also populate binary tree
void *dictionary_encode(void* thread_data) {
    // Declare variables
    struct ThreadPacket* packet = thread_data;
    
    // Populate binary tree
    int is_dup = tree_insert(packet->data, ENCODED_VALUE, &TREE_ROOT, (TreeCompare)tree_cmp_str);
    
    // Encode value to packet and update encoder if string is unique
    if (is_dup == 0) {
        packet->key = ENCODED_VALUE;
        ENCODED_VALUE++;
    } else { // Otherwise, use previously-encoded key
    	packet->key = tree_search(packet->data, TREE_ROOT, (TreeCompare)tree_cmp_str);
    }

    // End thread
    pthread_exit(NULL);
}

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
int main(int argc, char* argv[]) {
    /* =============================================================== */
    /* === DATABASE GENERATION === */
    clock_t begin = clock(); // Begin execution timer

    // Command-line error check.
    if (argc < 3 || detect_int(argv[2]) == 0) {
        fprintf(stderr, "ERROR:\tInvalid argument(s)\n"\
                        "USAGE:\tproj4.out <input_file> <num_threads>\n");
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    char* input_filename = argv[1];
    const int thread_limit = atoi(argv[2]);
    
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
    
    // Create array of thread IDs for each worker thread
    pthread_t thread_ids[thread_limit];

    // Create array of thread data for each column
    struct ThreadPacket* threads = malloc(thread_limit * sizeof(struct ThreadPacket));

    // Read in each column of data from the input file and spawn an encoder thread for it
    char line[1024];
    int row_num = 0;
    int active_threads = 0;
    while (fgets(line, sizeof(line) - 1, input_file)) {
    	// Remove trailing newline characters from strings
	if (line[strlen(line) - 1] = '\n') {
	    line[strlen(line) - 1] = '\0';
	}
        
        // Assign data to packet
        threads[row_num % thread_limit].data = malloc(strlen(line)+1);
        strcpy(threads[row_num % thread_limit].data, line);
        
        // Spawn thread to encode this column of data
        pthread_create(&thread_ids[row_num % thread_limit], NULL, dictionary_encode,\
        	       (void*)&threads[row_num % thread_limit]);
        
        // Update trackers
        row_num++;
        active_threads++;
        
        // If number of active threads is at the limit, wait for rejoin
    	if (active_threads >= thread_limit) {
    	    // Join thread
            pthread_join(thread_ids[row_num % thread_limit], NULL);
            
            // Write data to output file
            fprintf(output_file, "%u\t%s\n", threads[row_num % thread_limit].key,\
                    threads[row_num % thread_limit].data);
	    fflush(stdout);
	    
	    // Update tracker and free data
	    active_threads--;
            free(threads[row_num % thread_limit].data);
    	}
    }

    // Wait for remaining threads to finish encoding; output data to dictionary file
    for (int i = row_num % thread_limit + 1; i < (row_num % thread_limit) + thread_limit; i++) {
    	// Join thread
        pthread_join(thread_ids[i % thread_limit], NULL);
        
        // Write entry to output file
        fprintf(output_file, "%u\t%s\n", threads[i % thread_limit].key,\
        	threads[i % thread_limit].data);
	fflush(stdout);
	
	// Free data
        free(threads[i % thread_limit].data);
    }
	
    // Print execution time
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("ELAPSED TIME FOR DATABASE GENERATION:\n---> %f seconds\n", time_spent);
    
    /* =============================================================== */
    /* === QUERYING === */
    
    /* Once the database is generated, we can allow users to query it */
    
    // Prompt user for input and respond continuously
    char user_input;
    while(0) { // TBD, replace with user input like getchar() or something
        // Begin execution timer
    	begin = clock();
    	
    	// DO QUERYING STUFF HERE
    	/* i.e. we want to do the following from the project:
	    	(2) Query: enable users to query an existing encoded column file.
	    	Your implementation should allow users to
		(i) check whether one data item exists in the column, if it exists,
		return the indices of all the matching entries
		in the column; (ii) given a prefix, search and return all the unique
		matching data and their indices. Your
		implementation must support the use of SIMD instructions to speed up
		the search/scan.
		(3) Your code should also support the vanilla column search/scan
		(i.e., without using dictionary encoding),
		which will be used as a baseline for the speed performance comparison
	*/
	
	// The binary tree is set up correctly. Basically, you can look up values like this:
	char* search_term = "fabzighnd";
	ushort encoded_val = tree_search(search_term, TREE_ROOT, (TreeCompare)tree_cmp_str);
	
	// If encoded_val is 0, then the string does not exist in the tree
	if (encoded_val == 0) {
	    printf("%s does not exist!\n", search_term);
	}
    	
    	// Print execution time
    	end = clock();
    	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    	printf("ELAPSED TIME FOR QUERYING:\n---> %f seconds\n", time_spent);
    }
    
    // ----> TREE PRINT (FOR DEBUGGING)
    printf("\nBINARY TREE VALUES (in order):\n");
    tree_print(TREE_ROOT);

    /* =============================================================== */
    /* === END PROGRAM === */
    
    // Close files and clean up
    fclose(input_file);
    fclose(output_file);
    free(threads);
    tree_delete(&TREE_ROOT);
    
    // End program
    return EXIT_SUCCESS;
}

