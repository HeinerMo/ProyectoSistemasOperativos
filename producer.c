//Producer.c y
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include "semaphore.h"
#include <time.h>
#define PRODUCER_SEMAPHORE_NAME "/producerSemaphore"
#define CONSUMER_SEMAPHORE_NAME "/consumerSemaphore"

struct BufferData {
	long producerID;
	char date[80];
	int number;
	char message[1024];
};

struct GlobalData {
	int lastConsumed;
	int lastProduced;
	int activeProducers;
	int activeConsumers;
	int produce;
};

void endLine(char *message, int length) {

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	}    // for i

} // Fin de endLine


struct BufferData *produce(char message[1024]) {
	struct BufferData *bd;

	char *aux = strdup(message);
	strcpy(bd->message, message);
	bd->producerID = getpid();

	//using UNIX Epoch
	time_t now;
	struct tm ts; //TIME struct
	char currentTime[80];
	time(&now);
	ts = *localtime(&now);
	strftime(currentTime, sizeof(currentTime), "%d-%m-%Y %H:%M:%S", &ts);

	strcpy(bd->date, currentTime);

	bd->number  = rand() % 7;

	printf("PID: %d\n", bd->producerID);
	printf("Date: %s\n", &bd->date);
	printf("Number: %i\n", bd->number);
	printf("Message: %s\n", &bd->message);
}

struct GlobalData *loadGlobalData(int globalDataDescriptor) {

    struct GlobalData *globalData;
    struct stat shmobj_st;

    if (globalDataDescriptor == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if (fstat(globalDataDescriptor, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }

    globalData = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, globalDataDescriptor, 0);
    if (globalData == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }

    return globalData;

} // End of loadGlobalData

void writeProcess(int fd, struct BufferData *toWrite) {

	struct BufferData *buffer;
	

	if (fd == -1) {
		printf("Error file descriptor %s\n", strerror(errno));
		exit(1);
	}

	buffer = mmap(NULL, sizeof(toWrite), PROT_WRITE, MAP_SHARED, fd, 0);
	if (buffer == MAP_FAILED) {
		printf("Map failed in write process: %s\n", strerror(errno));
		exit(1);
	}
	
	int globalDataDescriptor = shm_open("GlobalData", O_RDWR, 00600);
	struct GlobalData *globalData = loadGlobalData(globalDataDescriptor);
	int i = globalData->lastProduced;
	i++;
	printf("Cantidad producida: %i\n", i);

	memcpy(&globalData->lastProduced, &i, sizeof(i));

//	printf("last: %i", globalData->lastProduced);


	puts("here");
	memcpy(buffer, toWrite, sizeof(buffer));

} // End of writeProcess

int main(int argc, char *argv[]) {
	/*TODO
		Subir producto a memoria compartida
		Recibir nombre por parámetro
	 */

	srand(time(NULL)); //Change random number seed
	int lenght = 1024;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	int fd = shm_open(buffName, O_RDWR, 00200); /* open s.m object*/
	//Open semaphores

	sem_t *consumers = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producers = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);

	while (1) {
		sem_wait(producers);
		//begining critical region
		char buf[lenght];
		printf("%s", "Enter some text: ");
		fgets(buf, lenght, stdin);
		endLine(buf, lenght);
		writeProcess(fd, produce(buf));
		//end critical region
		sem_post(consumers);
	} // while

	close(fd);

	return 0;
} // End of Class
