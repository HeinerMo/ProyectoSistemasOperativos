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
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/types.h>

// Arguments:
// -s [buffer size]
// -n [buffer name]

#define SIZEOF_SMOBJ 200
char * buffName;
int buffSize;

void checkParameters(int argc, char *argv[]){

	for (int i = 1; i < argc; i++) {
		//printf("Valor %i: %s\n", i, argv[i]); 
		if (strcmp(argv[i], "-s") == 0) {
			if (argv[i+1] != NULL) {
				buffSize = strtol(argv[i+1], NULL, 10);
				//printf("Buffer size: %d", buffSize);
			} // if
		} // if
		
		if (strcmp(argv[i], "-n") == 0) {
			if (argv[i+1] != NULL) {
				buffName = argv[i+1];
				//printf("Buffer name: %s\n", buffname);
			} // if
		} // if
	} // for i

} // Fin de checkParameters

void createSharedMemory () {

	int fd;
   	fd = shm_open (buffName, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
   	if(fd == -1) {
       printf("Error file descriptor \n");
	   exit(1);
   	} // if
   	
	   if(-1 == ftruncate(fd, SIZEOF_SMOBJ)) {
       printf("Error shared memory cannot be resized \n");
	   exit(1);
   	} // if
   
   	close(fd);

} // Fin de createSharedMemory

int main(int argc, char *argv[]) {

	puts("Initializer.c running...");
	pid_t pid = getpid();//Get process PID
	printf("pid: %u\n", pid);//prints PID

	//If the user enters -help show the argument sintax	
	if (strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -n \"buffer name\" -s \"[buffer size]\"");
		return 0;
	} // if

	//if there are too few arguments exit
	if (argc < 5) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		return 1;
	} // if
	
	//Decode arguments
	checkParameters(argc, argv);
	createSharedMemory();

	return 0;
} // Fin de main
