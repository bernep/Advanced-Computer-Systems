/* proj1.c */
/* Patrick Berne, Andre Shibata */
/* Example Command Line: "gcc -Wall -Werror proj1.c -pthread -o proj1.out" */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

/* Main Function */
int main(int argc, char **argv) {
	printf("Testing Testing\r\n");
	fflush(stdout);
	return EXIT_SUCCESS;
}
