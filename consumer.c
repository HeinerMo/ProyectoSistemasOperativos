//Consumer.c 
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

void endLine(char *message, int length) { //FIXME Borrar

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	} // for i

} // End of endLine

struct BufferData *loadBufferData (int bufferDataDescriptor) {

	struct BufferData *bufferData;
	struct stat shmobj_st;

	if (bufferDataDescriptor == -1)
	{
		printf("Error file descriptor %s\n", strerror(errno));
		exit(1);
	}

	if (fstat(bufferDataDescriptor, &shmobj_st) == -1)
	{
		printf(" error fstat \n");
		exit(1);
	}

	bufferData = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, bufferDataDescriptor, 0);
	if (bufferData == MAP_FAILED)
	{
		printf("Map failed in read process: %s\n", strerror(errno));
		exit(1);
	}

	return bufferData;

} // End of loadBufferData

struct GlobalData *loadGlobalData (int globalDataDescriptor) {

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

char* consume (int bufferDataDescriptor, int globalDataDescriptor) {

	struct BufferData *bufferData = loadBufferData(bufferDataDescriptor);
	struct GlobalData *globalData = loadGlobalData(globalDataDescriptor);
	struct BufferData *auxBufferData;

	int index = globalData->lastProduced; //TODO cargar el puntero de los consumidores de la memoria
	int lastConsumed = globalData->lastConsumed + 1;
	memcpy(globalData->lastConsumed, lastConsumed, sizeof(lastConsumed));

	auxBufferData = &bufferData[index];
	char data[2048];

	sprintf(data, "ID Productor: %s\n Hora y Fecha: %s\n Número mágico: %d\n",
		       	auxBufferData->producerID, auxBufferData->date, auxBufferData->number);
	char *temp = strdup(data);

	return temp;

} // End of consume


int main(int argc, char *argv[]) {

	puts("Initializer.c running...");
    pid_t pid = getpid();//Get process PID
    printf("pid: %u\n", pid);//prints PID

	/*TODO
	  Parámetros
	  Puntero de consumidor
	 */
	int lenght = 2048;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	int bufferDataDescriptor = shm_open(buffName, O_RDONLY, 00400); /* open s.m object*/
	int globalDataDescriptor = shm_open("GlobalData", O_RDONLY, 00600); 

	//Open semaphores

	sem_t *consumers = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producers = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);



	//Prueba
	//struct GlobalData *globalData;
	//globalData = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	

	while (1) {
		sem_wait(consumers);

		//inicio región crítica
		char* ptr = consume(bufferDataDescriptor, globalDataDescriptor);
		printf("%s \n", ptr);
		//fin región crítica

		sem_post(producers);
	} // while

	close(bufferDataDescriptor);

	return 0;

} // End of main