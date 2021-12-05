//Finisher.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <string.h>
#include <time.h>
#include "structures.h"
#include "semaphore.h"

char *bufferName;

// Verify that the parameters have been entered correctly
void checkParameters(int argc, char *argv[]){

	//If the user enters -help show the argument sintax	
	if (argc > 1 && strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -n \"Buffer name\"");
		exit(1);
	} // if

	//if there are too few arguments exit
	if (argc < 3) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		exit(1);
	} // if


	for (int i = 1; i < argc; i++) {
		
		if (strcmp(argv[i], "-n") == 0) {
			if (argv[i+1] != NULL) {
				bufferName = argv[i+1];
			} // if
		} // if

	} // for i

} // Fin de checkParameters

// Load shared global data memory
GlobalData *loadGlobalData(int globalDataDescriptor) {

   	if (globalDataDescriptor == -1) {
    	printf("There is no shared memory called \"%s\"\n", bufferName);
      	exit(1);
   	}

	int memorySize = sizeof(GlobalData);
   	GlobalDataPointer globalData = mmap(NULL, memorySize, PROT_WRITE, MAP_SHARED, globalDataDescriptor, 0);
   	if (globalData == MAP_FAILED) {
      	printf("Map failed in write process: %s\n", strerror(errno));
      	exit(1);
   	}

    return globalData;

} // End of loadGlobalData

// Load shared memory buffer
BufferData *loadBufferData(int bufferDataDescriptor, int bufferSize) {

   	if (bufferDataDescriptor == -1) {
    	printf("There is no shared memory called \"%s\"\n", bufferName);
      	exit(1);
   	}

	int memorySize = bufferSize * sizeof(BufferData);
   	BufferDataPointer bufferData = mmap(NULL, memorySize, PROT_WRITE, MAP_SHARED, bufferDataDescriptor, 0);
   	if (bufferData == MAP_FAILED) {
      	printf("Map failed in write process: %s\n", strerror(errno));
      	exit(1);
   	}

    return bufferData;

} // End of loadBufferData

// Get the time since 00:00:00 UTC, January 1, 1970 in seconds
long getTimeSec () {

	time_t timeNow;
	return time(&timeNow);

} // End getTime

int main(int argc, char *argv[]) {
	
	//Decode arguments
	checkParameters(argc, argv);

	//Open semaphores
	sem_t *consumersSem = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producersSem = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);

	int bufferDataDescriptor = shm_open(bufferName, O_RDWR, 00200); 
	int globalDataDescriptor = shm_open(GLOBAL_DATA_SHM_NAME, O_RDWR, 00200); 
	GlobalDataPointer globalData = loadGlobalData(globalDataDescriptor);
	BufferDataPointer bufferData = loadBufferData(bufferDataDescriptor, globalData->bufferSize);

	globalData->stateSignal = 1;

	while(1) {
		
		if (globalData->activeConsumers == 0) {
			sem_wait(consumersSem);
			sem_post(producersSem);
		}

		if (globalData->activeConsumers == 0 && globalData->activeProducers == 0) {
			char instruction[1024];
			sprintf(instruction, "rm /dev/shm/%s", bufferName);
			system(instruction);
			sprintf(instruction, "rm /dev/shm/%s", GLOBAL_DATA_SHM_NAME);
			system(instruction);
			sprintf(instruction, "rm /dev/shm/sem.%s", CONSUMER_SEMAPHORE_NAME);
			system(instruction);
			sprintf(instruction, "rm /dev/shm/sem.%s", PRODUCER_SEMAPHORE_NAME);
			system(instruction);
			sprintf(instruction, "rm /dev/shm/sem.%s", WRITE_SEMAPHORE_NAME);
			system(instruction);
			break;
		}

	} // while

	printf(COLOR_GREEN "\n====Todo se finalizó correctamente====\n\n" COLOR_RESET
			"Mensajes totales: %i\n"
			"Total de productores: %i\n" 
			"Total de consumidores: %i\n"
			"Consumidores eliminados por llave: %i\n"
			"Tiempo esperando total: %ld segundos\n"
			"Tiempo de usuario total: %ld segundos\n"
			"Tiempo total de bloqueos por semáforos: %ld segundos\n"  
		  COLOR_GREEN "=======================================\n\n" COLOR_RESET,
			globalData->totalMsgCount,
			globalData->producerTotal,
			globalData->consumersTotal,
			globalData->deletedByKey,
			globalData->totalWaitTime,
			((getTimeSec() - globalData->totalUserTime)+1),
			globalData->totalBlockedTime
		);

	return 0;
} // End of main
