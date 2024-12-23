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
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <sys/types.h>
#include <errno.h>

// Arguments:
// -s [buffer size]
// -n [buffer name]

#define PRODUCER_SEMAPHORE_NAME "/producerSemaphore"
#define CONSUMER_SEMAPHORE_NAME "/consumerSemaphore"
char * buffName;
int buffSize;

struct BufferData {
	char producerID[50];
	char date[20];
	char time[20];
	int number;
};

struct GlobalData {
	int lastConsumed;
	int lastProduced;
	int activeProducers;
	int activeConsumers;
	int produce;
};

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

void createSharedMemory (struct BufferData *buffer) {

	int fd;
	fd = shm_open (buffName, O_CREAT | O_RDWR  , 00700); /* create s.m object*/
	if(fd == -1) {
		printf("Error file descriptor \n");
		exit(1);
	} // if

	if(-1 == ftruncate(fd, sizeof(buffer))) {
		printf("Error shared memory cannot be resized \n");
		exit(1);
	} // if

	close(fd);

} // Fin de createSharedMemory

void createGlobalData() {

	struct GlobalData *data;
	data->produce = 1;

	int fd;
	fd = shm_open ("GlobalData", O_CREAT | O_RDWR  , 00700); /* create s.m object*/
	if(fd == -1) {
		printf("Error file descriptor \n");
		exit(1);
	} // if

	if(-1 == ftruncate(fd, sizeof(data))) {
		printf("Error shared memory cannot be resized \n");
		exit(1);
	} // if
	
	struct GlobalData *aux;

	aux = mmap(NULL, sizeof(data), PROT_WRITE, MAP_SHARED, fd, 0);

	if (data == MAP_FAILED) {
		printf("Map failed in write process: %s\n", strerror(errno));
		exit(1);	
	}

	memcpy(aux, data, sizeof(data));


	close(fd);


}//end createGlobalData


void createSemaphores() {
	//Producer semaphore
	sem_t *producerSemaphore = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, buffSize);
	sem_t *consumerSemaphore = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 0);	
}

int main(int argc, char *argv[]) {
	
	//Decode arguments
	checkParameters(argc, argv);

	//Create local buffer pointer
	struct BufferData buffer[buffSize];

	//Create shared resouces
	createSharedMemory(buffer);
	createSemaphores();
	createGlobalData();

	//print process information
	puts("Initializer.c running...");
	pid_t pid = getpid();//Get process PID
	printf("Buffer name: %s", buffName);
	printf("Buffer size: %i\n", buffSize);
	printf("sizeof(buffer) %i\n", sizeof(buffer));

	return 0;
} // Fin de main
