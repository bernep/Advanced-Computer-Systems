/* proj4.c */
/* Patrick Berne, Andre Shibata */

// Example Compile Line: "gcc proj4.c bin_tree.c -o proj4.out -mavx2"
// Example Execute Line: "./proj4.out col.txt 10 1"

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
    uint key;
    uint index;
    char* data;
};

// Global variables
struct TreeNode* TREE_ROOT = NULL;
uint THREAD_LIMIT = 10;
uint QUERY_PRINT_ENABLE = 0;
uint ENCODED_VALUE = 1;
pthread_mutex_t MUTEX;

// Function: Dictionary encoder as a worker thread; also populate binary tree
void *dictionary_encode(void* thread_data) {
    // Declare variables
    struct ThreadPacket* packet = thread_data;
    int is_dup = 0;
    
    // Populate binary tree
    /* The first few entries have some multithreading accessing issues due to how they join
       compared to all subsequent threads, so lock the tree for them */
    if (packet->index < THREAD_LIMIT) {
        pthread_mutex_lock(&MUTEX);
        is_dup = tree_insert(packet->data, packet->index, ENCODED_VALUE, &TREE_ROOT);
        pthread_mutex_unlock(&MUTEX);
    } 
    // Otherwise, we can just multithread as needed
    else {
        is_dup = tree_insert(packet->data, packet->index, ENCODED_VALUE, &TREE_ROOT);
    }
    
    // Encode value to packet and update encoder if string is unique
    if (is_dup == 0) {
        packet->key = ENCODED_VALUE;
        ENCODED_VALUE++;
    } else { // Otherwise, use previously-encoded key
    	packet->key = tree_str_search(packet->data, TREE_ROOT);
    }

    // End thread
    pthread_exit(NULL);
}

// Function: Search raw column data file for specified search term (naive)
// ---> Note: file_to_read is assumed to be open in read-only mode
void vanilla_full_search(FILE* file_to_read, char* search_term){
    // Variables
    char line[MAX_STR_LEN];
    int row_index = 0;
    int match_found = 0;

    // Search file
    while (fgets(line, sizeof(line) - 1, file_to_read)) {
        // Remove trailing newline characters
        line[strlen(line) - 1] = '\0';

        // Check for match and print if it does match (first match)
        if (match_found == 0 && strcmp(line, search_term) == 0) {
            if (QUERY_PRINT_ENABLE == 1) {
                printf("\n\nSearch term \"%s\" found at the following lines:\n", search_term);
            } else {
                printf("\n\nSearch term \"%s\" found!\n", search_term);
            }
            match_found = 1;
        }

        // Check for match and print if it does match (subsequent matches)
        if (match_found == 1 && strcmp(line, search_term) == 0) {
            if (QUERY_PRINT_ENABLE == 1) printf("    %d\n", row_index+1);
        }

        // Update index
        row_index++;
    }

    // Print failure if nothing found
    if (match_found == 0) {
        printf("\n\nSearch term \"%s\" was not found in the file.\n", search_term);
    }
}

// Function: Search raw column data file using prefix to find matching terms (naive)
// ---> Note: file_to_read is assumed to be open in read-only mode
void vanilla_prefix_search(FILE* file_to_read, char* prefix){
    // Variables
    char line[MAX_STR_LEN];
    int row_index = 0;
    int match_found = 0;

    // Search file
    while (fgets(line, sizeof(line) - 1, file_to_read)) {
        // Remove trailing newline characters
        line[strlen(line) - 1] = '\0';

        // Check for match and print if it does match (first match)
        if (match_found == 0 && strncmp(line, prefix, strlen(prefix)) == 0) {
            if (QUERY_PRINT_ENABLE == 1) {
                printf("\n\nPrefix \"%s\" found at the following lines:\n", prefix);
            } else {
                printf("\n\nPrefix \"%s\" found!\n", prefix);
            }
            match_found = 1;
        }

        // Check for match and print if it does match (subsequent matches)
        if (match_found == 1 && strncmp(line, prefix, strlen(prefix)) == 0) {
            if (QUERY_PRINT_ENABLE == 1) printf("    %d:    \t%s\n", row_index+1, line);
        }

        // Update index
        row_index++;
    }

    // Print failure if nothing found
    if (match_found == 0) {
        printf("\n\nPrefix \"%s\" was not found in the file.\n", prefix);
    }
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
    /* === INITIALIZATION === */
    /* Set up file system and other initial variables for database generation */

    // Command-line error check.
    if (argc < 4 || detect_int(argv[2]) == 0 || detect_int(argv[3]) == 0\
                 || atoi(argv[3]) < 0 || atoi(argv[3]) > 1) {
        fprintf(stderr, "ERROR:\tInvalid argument(s)\n"\
                        "USAGE:\tproj4.out <input_file> <num_threads> <query_print_enable>\n");
        return EXIT_FAILURE;
    }

    // Grab command-line arguments
    char* input_filename = argv[1];
    THREAD_LIMIT = atoi(argv[2]);
    QUERY_PRINT_ENABLE = atoi(argv[3]);
    
    // Check if input file exists, and create a pointer to it
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "ERROR: Input file missing\n");
        return EXIT_FAILURE;
    }
    
    // Create output file and its pointer
    char output_filename[MAX_STR_LEN];
    strcpy(output_filename, "dict_");
    strcat(output_filename, input_filename);
    FILE* output_file = fopen(output_filename, "w");
    
    // Create arrays for of thread IDs and thread datafor each worker thread
    pthread_t thread_ids[THREAD_LIMIT];
    struct ThreadPacket* threads = malloc(THREAD_LIMIT * sizeof(struct ThreadPacket));

    // Print separator
    printf("\n=====================================================\n\n");

    /* === DATABASE GENERATION === */
    /* Generate an encoded dictionary that the user can then query */

    // Print startup and begin timer
    printf("Generating database... please wait (this can take awhile).\n");
    clock_t begin = clock();

    // Read in input data, spawn encoder thread for each line, and write data to output file
    char line[MAX_STR_LEN];
    int row_num = 0;
    int active_threads = 0;
    while (fgets(line, sizeof(line) - 1, input_file)) {
    	// Remove trailing newline characters from strings
        line[strlen(line) - 1] = '\0';
        
        // Assign data to packet
        threads[row_num % THREAD_LIMIT].data = malloc(strlen(line)+1);
        strcpy(threads[row_num % THREAD_LIMIT].data, line);
        threads[row_num % THREAD_LIMIT].index = row_num + 1;
        
        // Spawn thread to encode this column of data
        pthread_create(&thread_ids[row_num % THREAD_LIMIT], NULL, dictionary_encode,\
        	           (void*)&threads[row_num % THREAD_LIMIT]);
        
        // Update trackers
        row_num++;
        active_threads++;
        
        // If number of active threads is at the limit, wait for rejoin
    	if (active_threads >= THREAD_LIMIT) {
            // Join thread
            pthread_join(thread_ids[row_num % THREAD_LIMIT], NULL);
            
            // Write data to output files
            fprintf(output_file, "%u\t%s\n", threads[row_num % THREAD_LIMIT].key,\
                    threads[row_num % THREAD_LIMIT].data);
            fflush(stdout);
    	    
    	    // Update tracker and free data
    	    active_threads--;
            free(threads[row_num % THREAD_LIMIT].data);
    	}
    }

    // Wait for remaining threads to finish encoding; output data to dictionary file
    for (int i = row_num % THREAD_LIMIT + 1; i < (row_num % THREAD_LIMIT) + THREAD_LIMIT; i++) {
    	// Join thread
        pthread_join(thread_ids[i % THREAD_LIMIT], NULL);
        
        // Write entry to output file
        fprintf(output_file, "%u\t%s\n", threads[i % THREAD_LIMIT].key,\
        	    threads[i % THREAD_LIMIT].data);
	    fflush(stdout);
	
	    // Free data
        free(threads[i % THREAD_LIMIT].data);
    }
	
    // Print execution time for database generation
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("\nELAPSED TIME FOR DATABASE GENERATION:\n---> %f seconds\n", time_spent);

    // Close files and clean up
    fclose(input_file);
    fclose(output_file);
    free(threads);

    // Print separator
    printf("\n=====================================================\n\n");
    
    /* === QUERYING === */   
    /* Once the database is generated, we can allow users to query it */

    // Print instructions for user
    printf("The dictionary has been generated.\n"\
           "To query the database or exit the program, enter:\n"\
           "    1 for Vanilla Full-Term Column Search\n"\
           "    2 for Vanilla Prefix Column Search\n"\
           "    3 for Encoded/SIMD Full-Term Column Search\n"\
           "    4 for Encoded/SIMD Prefix Column Search\n"\
           "    0 to Exit Program\n");

    // Print separator
    printf("\n=====================================================\n\n");

    // Prompt user for input and query as needed in a continuous loop
    char user_input[MAX_STR_LEN];
    user_input[0] = '8';
    while (strcmp(user_input, "0") != 0) {
        // Get user input
        printf("Please enter 1, 2, 3, 4, or 0: ");   
        scanf("%s", user_input);

        // Vanilla full-term column search
        if (strcmp(user_input, "1") == 0) {
            // Get input
            char search_term[MAX_STR_LEN];
            printf("\n=== VANILLA FULL-TERM COLUMN SEARCH ===\n"\
                   "Please enter the string you are looking for: ");
            scanf("%s", search_term);

            // Begin timer, open file
            printf("\nSearching file... If file is large, please wait.");
            if (QUERY_PRINT_ENABLE == 0) {
                printf("\nNote: If indices are found, they will not print.");
            }
            begin = clock();
            FILE* file_to_read = fopen(input_filename, "r");

            // Scan file
            vanilla_full_search(file_to_read, search_term);

            // Close file, print elapsed time
            fclose(file_to_read);
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("\nELAPSED TIME FOR QUERY:\n---> %f seconds\n", time_spent);
        }

        // Vanilla prefix column search
        else if (strcmp(user_input, "2") == 0) {
            // Get input
            char search_term[MAX_STR_LEN];
            printf("\n=== VANILLA PREFIX COLUMN SEARCH ===\n"\
                   "Please enter the prefix you are looking for: ");
            scanf("%s", search_term);

            // Begin timer, open file
            printf("\nSearching file... If file is large, please wait.");
            if (QUERY_PRINT_ENABLE == 0) {
                printf("\nNote: If indices are found, they will not print.");
            }
            begin = clock();
            FILE* file_to_read = fopen(input_filename, "r");

            // Scan file
            vanilla_prefix_search(file_to_read, search_term);

            // Close file, print elapsed time
            fclose(file_to_read);
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("\nELAPSED TIME FOR QUERY:\n---> %f seconds\n", time_spent);
        }

        // Encoded/SIMD full-term column search
        else if (strcmp(user_input, "3") == 0) {
            // Get input
            char search_term[MAX_STR_LEN];
            printf("\n=== ENCODED/SIMD FULL-TERM COLUMN SEARCH ===\n"\
                   "Please enter the string you are looking for: ");
            scanf("%s", search_term);

            // Begin timer
            begin = clock();

            // Search for matching string, obtain its code
            uint code = tree_str_search(search_term, TREE_ROOT);

            // For each matching code in the dictionary, print their index
            if (code > 0) {
                if (QUERY_PRINT_ENABLE == 1) {
                    printf("\nMatching string(s) for search term \"%s\" found at "\
                           "the following lines:\n", search_term);
                } else {
                    printf("\nMatching string(s) for search term \"%s\" found!\n", search_term);
                    printf("Please wait for the query to find indices.\n");
                    printf("Note: The indices will not print.\n");
                    printf("\nSearching...\n");
                }
                struct ListNode* indices = tree_get_indices(code, TREE_ROOT);
                if (indices != NULL) {
                    if (QUERY_PRINT_ENABLE == 1) printf("    %d\n", indices->index);
                    while (indices->next != NULL) {
                        indices = indices->next;
                        if (QUERY_PRINT_ENABLE == 1) printf("    %d\n", indices->index);
                    }
                }
            } else {
                printf("\nNo matching strings for search term \"%s\".\n", search_term);
            }

            // Print elapsed time
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("\nELAPSED TIME FOR QUERY:\n---> %f seconds\n", time_spent);
        }

        // Encoded/SIMD prefix column search
        else if (strcmp(user_input, "4") == 0) {
            // Get input
            char search_term[MAX_STR_LEN];
            printf("\n=== ENCODED/SIMD PREFIX COLUMN SEARCH ===\n"\
                   "Please enter the prefix you are looking for: ");
            scanf("%s", search_term);

            // Begin timer
            begin = clock();

            // Setup variables for searching
            char result[MAX_STR_LEN];
            uint* codes = malloc(ENCODED_VALUE*sizeof(uint)); // Overkill, but simple
            size_t codes_size = 0;

            // Search for strings with matching prefix, store their codes in codes[]
            tree_prefix_search(search_term, codes, &codes_size, TREE_ROOT);

            // For each matching code, print their matching dictionary entry
            if (codes_size > 0) {
                // Check if entry exists in binary tree
                if (QUERY_PRINT_ENABLE == 1) {
                    printf("\nMatching string(s) for prefix \"%s\" found:\n", search_term);
                } else {
                    printf("\nMatching string(s) for prefix \"%s\" found!\n", search_term);
                    printf("Please wait for the query to find indices.\n");
                    printf("Note: The indices will not print.\n");
                    printf("\nSearching...\n");
                }
                for (int i = 0; i < codes_size; i++) {
                    tree_code_search(codes[i], result, TREE_ROOT);
                    if (QUERY_PRINT_ENABLE == 1) printf("\t%s\n", result);
                }

                // For each matching code in the dictionary, print their index
                if (codes_size > 0) {
                    if (QUERY_PRINT_ENABLE == 1) {
                        printf("\nMatching string(s) for prefix \"%s\" found at "\
                               "the following lines:\n", search_term);
                    }
                    // Iterate through all the valid codes
                    for (int i = 0; i < codes_size; i++) {
                        tree_code_search(codes[i], result, TREE_ROOT);
                        if (QUERY_PRINT_ENABLE == 1) printf("\n    %s:\n", result);
                        struct ListNode* indices = tree_get_indices(codes[i], TREE_ROOT);
                        if (QUERY_PRINT_ENABLE == 1) printf("        %d\n", indices->index);
                        while (indices->next != NULL) {
                            indices = indices->next;
                            if (QUERY_PRINT_ENABLE == 1) printf("        %d\n", indices->index);
                        }
                    }
                }
            } else {
                printf("\nNo matching strings for prefix \"%s\".\n", search_term);
            }

            // Free memory
            free(codes); 

            // Print elapsed time
            end = clock();
            time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("\nELAPSED TIME FOR QUERY:\n---> %f seconds\n", time_spent);
        }

        // Invalid entry
        else if (strcmp(user_input, "0") != 0) {
            // Print invalid reponse
            printf("Invalid entry; please re-enter input.\n");
        }

        // Print separator
        printf("\n=====================================================\n\n");
    }

    /* === END PROGRAM === */
    /* Clean up and end program */
    tree_delete(&TREE_ROOT);
    return EXIT_SUCCESS;
}
