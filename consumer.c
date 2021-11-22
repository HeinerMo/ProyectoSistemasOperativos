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



void endLine(char *message, int length) { //FIXME Borrar

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	} // for i

} // End of endLine

char* readProcess (int fd) {

	struct BufferData *buffer;
	struct stat shmobj_st;
	struct BufferData *aux;

	if (fd == -1)
	{
		printf("Error file descriptor %s\n", strerror(errno));
		exit(1);
	}

	if (fstat(fd, &shmobj_st) == -1)
	{
		printf(" error fstat \n");
		exit(1);
	}
	buffer = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (buffer == MAP_FAILED)
	{
		printf("Map failed in read process: %s\n", strerror(errno));
		exit(1);
	}

	int index = 0; //TODO obtener puntero para memoria circuar y calcular índice
	aux = &buffer[index]; 
	char data[2048];

	sprintf(data, "ID Productor: %s\n Fecha: %s\n Tiempo: %s\n Número mágico: %d\n",
		       	aux->producerID, aux->date, aux->time, aux->number);
	char *temp = strdup(data);
	return temp;

} // End of readProcess

int main(int argc, char *argv[]) {

	/*TODO
	  Parámetros (Nombre buffer)
	  Puntero de consumidor
	 */
	int lenght = 2048;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	int fd = shm_open(buffName, O_RDONLY, 00400); /* open s.m object*/


	//Open semaphores

	sem_t *consumers = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producers = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);



	//Prueba
	//struct GlobalData *globalData;
	//globalData = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	struct GlobalData *globalData;
	int globalDataDescriptor = shm_open("GlobalData", O_RDWR, 00600);	
	
	globalData = mmap(NULL, sizeof(globalData), PROT_WRITE, MAP_SHARED, globalDataDescriptor, 0);	

	printf("GlobalData->lastConsumed: %i\n", globalData->lastConsumed);
	printf("GlobalData->lastProduced: %i\n", globalData->lastProduced);
	printf("GlobalData->activeProducers: %i\n", globalData->activeProducers);
	printf("GlobalData->activeConsumers: %i\n", globalData->activeConsumers);
	printf("GlobalData->produce: %i\n", globalData->produce);

	while (1) {
		sem_wait(consumers);

		//inicio región crítica
		char* ptr = readProcess(fd);
		printf("%s \n", ptr);
		//fin región crítica

		sem_post(producers);
	} // while

	close(fd);

	return 0;

} // End of main
