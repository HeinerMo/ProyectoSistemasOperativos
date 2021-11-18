//Initializer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>


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
   int fd;
   fd = shm_open (buffname, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
   if(fd == -1)
   {
       printf("Error file descriptor \n");
	   exit(1);
   }
   if(-1 == ftruncate(fd, buffSize))
   {
       printf("Error shared memory cannot be resized \n");
	   exit(1);
   }
   
   close(fd);
   
   return 0;
}







