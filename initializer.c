//Initializer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "semaphore.h"
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#include "structures.h"

// Arguments:
// -s [buffer size]
// -n [buffer name]

char * bufferName;
int bufferSize;

// Verify that the parameters have been entered correctly
void checkParameters(int argc, char *argv[]){

	//If the user enters -help show the argument sintax	
	if (argc > 1 && strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -n \"buffer name\" -s \"[buffer size]\"");
		exit(1);
	} // if

	//if there are too few arguments exit
	if (argc < 5) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		exit(1);
	} // if


	for (int i = 1; i < argc; i++) {
		//printf("Valor %i: %s\n", i, argv[i]); 
		if (strcmp(argv[i], "-s") == 0) {
			if (argv[i+1] != NULL) {
				bufferSize = strtol(argv[i+1], NULL, 10);
				//printf("Buffer size: %d", buffSize);
			} // if
		} // if

		if (strcmp(argv[i], "-n") == 0) {
			if (argv[i+1] != NULL) {
				bufferName = argv[i+1];
				//printf("Buffer name: %s\n", buffname);
			} // if
		} // if
	} // for i

} // Fin de checkParameters

// Create the buffer in shared memory
void createSharedMemory () {

	int bufferDataDescriptor;
	bufferDataDescriptor = shm_open (bufferName, O_CREAT | O_RDWR  , 00700); 
	if(bufferDataDescriptor == -1) {
		printf("Error file descriptor to Buffer Data\n");
		exit(1);
	} // if

	int memorySize = bufferSize * sizeof(BufferData);
	if(-1 == ftruncate(bufferDataDescriptor, memorySize)) {
		printf("Error shared memory cannot be resized \n");
		exit(1);
	} // if

	close(bufferDataDescriptor);

} // Fin de createSharedMemory

// Get the time since 00:00:00 UTC, January 1, 1970 in seconds
long getTimeSec () {

	time_t timeNow;
	return time(&timeNow);

} // End getTime

// Create shared global data memory
void createGlobalData() {

	int globalDataDescriptor;
	globalDataDescriptor = shm_open (GLOBAL_DATA_SHM_NAME, O_CREAT | O_RDWR  , 00700);
	if(globalDataDescriptor == -1) {
		printf("Error file descriptor to Global Data\n");
		exit(1);
	} // if

	int memorySize = sizeof(GlobalData);
	if(-1 == ftruncate(globalDataDescriptor, memorySize)) {
		printf("Error shared memory cannot be resized \n");
		exit(1);
	} // if

	GlobalDataPointer globalData = mmap(NULL, memorySize, PROT_WRITE, MAP_SHARED, globalDataDescriptor, 0);
   	if (globalData == MAP_FAILED) {
      	printf("Map failed in write process: %s\n", strerror(errno));
      	exit(1);
   	}

	globalData->bufferSize = bufferSize;
	globalData->lastProduced = bufferSize - 1; 
	globalData->stateSignal = 0;
	globalData->totalUserTime = getTimeSec();

	close(globalDataDescriptor);

} // End createGlobalData

void createSemaphores() {
	
	//Producer semaphore
	sem_t *producerSemaphore = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, bufferSize);
	sem_t *consumerSemaphore = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 0);
	sem_t *writeSemaphore = sem_open(WRITE_SEMAPHORE_NAME, O_CREAT, 0644, 1);

} // End of createSemaphores

int main(int argc, char *argv[]) {	

	//Decode arguments
	checkParameters(argc, argv);

	//Create shared resouces
	createSharedMemory();
	createSemaphores();
	createGlobalData();
	
	return 0;
} // Fin de main
