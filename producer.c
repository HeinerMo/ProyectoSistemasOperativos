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

#include "structures.h"

void endLine(char *message, int length) {

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	}    // for i

} // Fin de endLine

void produce(BufferDataPointer bufferData, char message[1024]) {
	
	char *aux = strdup(message);
	strcpy(bufferData->message, message);
	bufferData->producerID = getpid();

	//using UNIX Epoch
	time_t now;
	struct tm ts; //TIME struct
	char currentTime[80];
	time(&now);
	ts = *localtime(&now);
	strftime(currentTime, sizeof(currentTime), "%d-%m-%Y %H:%M:%S", &ts);

	strcpy(bufferData->date, currentTime);

	bufferData->number  = rand() % 7;

} // End of produce

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

void writeProcess(BufferDataPointer bufferData, GlobalDataPointer globalData, char message[1024]) {
	
	globalData->lastProduced = (globalData->lastProduced + 1) % globalData->bufferSize;
	int index = globalData->lastProduced; 
	
	BufferData *auxBufferData = &bufferData[index];

	produce(auxBufferData, message);

	BufferData *auxBufferDataDos = &bufferData[index]; // BORRAR
	printf("PID: %ld\n", auxBufferDataDos->producerID); // BORRAR
	printf("Date: %s\n", strdup(auxBufferDataDos->date)); // BORRAR
	printf("Number: %i\n", auxBufferDataDos->number); // BORRAR
	printf("Message: %s\n", strdup(auxBufferDataDos->message)); // BORRAR

} // End of writeProcess

int main(int argc, char *argv[]) {
	/*TODO
		Subir producto a memoria compartida

		Recibir nombre por parácompartida
	 */

	srand(time(NULL)); //Change random number seed
	int lenght = 1024;
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

	while (1) {
		sem_wait(producersSem);
		//begining critical region
		char message[lenght];
		printf("%s", "Enter some text: ");
		fgets(message, lenght, stdin);
		endLine(message, lenght);
		writeProcess(bufferData, globalData, message);
		//end critical region
		sem_post(consumersSem);
	} // while

	close(globalDataDescriptor);
	close(bufferDataDescriptor);

	return 0;
} // End of Class
