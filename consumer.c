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
#include <time.h>

#include "structures.h"

void endLine(char *message, int length) { //FIXME Borrar

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	} // for i

} // End of endLine

GlobalData *loadGlobalData(int globalDataDescriptor) {

   	if (globalDataDescriptor == -1) {
    	printf("Error file descriptor %s\n", strerror(errno));
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

// FIXME CAMBIAR LOS PERMISOS A SÓLO LECTURA
BufferData *loadBufferData(int bufferDataDescriptor, int bufferSize) {

   	if (bufferDataDescriptor == -1) {
    	printf("Error file descriptor %s\n", strerror(errno));
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

char* consume (GlobalDataPointer globalData, BufferDataPointer bufferData, sem_t *writeSem) {

	BufferData *auxBufferData;

	int index = globalData->lastConsumed;
	globalData->lastConsumed = (globalData->lastConsumed + 1) % globalData->bufferSize;

	auxBufferData = &bufferData[index];
	char data[2048];

	sprintf(data, "ID Productor: %ld\n"
					"Hora y Fecha: %s\n"
					"Número mágico: %d \n"
					"Consumidores activos: %i\n"	
					"Productores activos: %i\n",
		       		auxBufferData->producerID, 
					auxBufferData->date, 
					auxBufferData->number, 
					globalData->activeConsumers,
					globalData->activeProducers);

    pid_t pid = getpid();
	if (globalData->stateSignal==1 || (pid%10) == auxBufferData->number) {
		return "end";
	}

	char *bufferMessage = strdup(data);
	return bufferMessage;

} // End of consume

long getTimeSec () {

	time_t timeNow;
	return time(&timeNow);

} // End getTime


int main(int argc, char *argv[]) {
	/*TODO
	  Parámetros
	 */
	int lenght = 2048;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	int bufferDataDescriptor = shm_open(buffName, O_RDWR, 00200); 
	int globalDataDescriptor = shm_open(GLOBAL_DATA_SHM_NAME, O_RDWR, 00200); 
	GlobalDataPointer globalData = loadGlobalData(globalDataDescriptor);
	BufferDataPointer bufferData = loadBufferData(bufferDataDescriptor, globalData->bufferSize);

	//Open semaphores
	sem_t *consumersSem = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producersSem = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *writeSem = sem_open(WRITE_SEMAPHORE_NAME, O_CREAT, 0644, 3);

	//Increase the number of active consumers in the global counter
	sem_wait(writeSem);		
	globalData->activeConsumers += 1;
	globalData->consumersTotal += 1;
	sem_post(writeSem);

	while (1) {

		long startTime = getTimeSec();		

		sem_wait(consumersSem);

		sem_wait(writeSem);	// Wait writeSem
		globalData->totalWaitTime += getTimeSec() - startTime;
		sem_post(writeSem); // Post writeSem

		char* bufferMessage = consume(globalData, bufferData, writeSem);
		
		if (strcmp(bufferMessage, "end") == 0) {
			break;
		} else {
			printf("%s \n", bufferMessage);
		}

		sem_post(producersSem);
		
	} // while

	sem_wait(writeSem);
	//Decrease the number of active producers on the global counter
	globalData->activeConsumers -= 1;
	sem_post(writeSem);

	close(bufferDataDescriptor);

	return 0;

} // End of main