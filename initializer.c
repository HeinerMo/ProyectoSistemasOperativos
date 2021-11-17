//Initializer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

// Arguments:
// -s [buffer size]
// -n [buffer name]


int main(int argc, char *argv[]) {
	puts("Initializer.c running...");
	pid_t pid = getpid();//Get process PID
	printf("pid: %lu\n", pid);//prints PID

	//If the user enters -help show the argument sintax	
	if (strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -n \"buffer name\" -s \"[buffer size]\"");
		return 0;
	}

	//if there are too few arguments exit
	if (argc < 5) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		return 1;
	}
	
	//Decode arguments
	
	int buffSize; //BufferSize
	char *buffname; //bufferName

	for (int i = 1; i < argc; i++) {
		printf("Valor %i: %s\n", i, argv[i]); 
		if (strcmp(argv[i], "-s") == 0) {
			if (argv[i+1] != NULL) {
				buffSize = strtol(argv[i+1], NULL, 10);
				printf("Buffer size: %i");
			}
		}
		
		if (strcmp(argv[i], "-n") == 0) {
			if (argv[i+1] != NULL) {
				buffname = argv[i+1];
				printf("Buffer name: %s\n", buffname);
			}
		}
	}
	return 0;
}
