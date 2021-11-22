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
	struct BufferData *auxBufferData;

	char *aux = strdup(message);
	strcpy(auxBufferData->message, message);
	auxBufferData->producerID = getpid();

	//using UNIX Epoch
	time_t now;
	struct tm ts; //TIME struct
	char currentTime[80];
	time(&now);
	ts = *localtime(&now);
	strftime(currentTime, sizeof(currentTime), "%d-%m-%Y %H:%M:%S", &ts);

	strcpy(auxBufferData->date, currentTime);

	auxBufferData->number  = rand() % 7;

	return auxBufferData;
} // End of produce

struct GlobalData *loadGlobalData(int globalDataDescriptor) {

	struct GlobalData *globalData;

   	if (globalDataDescriptor == -1) {
    	printf("Error file descriptor %s\n", strerror(errno));
      	exit(1);
   	}

   	globalData = mmap(NULL, sizeof(globalData), PROT_WRITE, MAP_SHARED, globalDataDescriptor, 0);
   	if (globalData == MAP_FAILED) {
      	printf("Map failed in write process: %s\n", strerror(errno));
      	exit(1);
   	}

    return globalData;

} // End of loadGlobalData

struct BufferData *loadBufferData(int bufferDataDescriptor) {

	struct BufferData *bufferData;

   	if (bufferDataDescriptor == -1) {
    	printf("Error file descriptor %s\n", strerror(errno));
      	exit(1);
   	}

   	bufferData = mmap(NULL, sizeof(bufferData), PROT_WRITE, MAP_SHARED, bufferDataDescriptor, 0);
   	if (bufferData == MAP_FAILED) {
      	printf("Map failed in write process: %s\n", strerror(errno));
      	exit(1);
   	}

    return bufferData;

} // End of loadBufferData

void writeProcess(struct BufferData *bufferData, struct GlobalData *globalData, struct BufferData *toWrite) {
	
	int index = globalData->lastProduced; // CORREGIR PUNTEROS PARA LO DEL "COSO" CIRCULAR

	struct BufferData *auxBufferData = &bufferData[index];

	auxBufferData->producerID = toWrite->producerID;
	strcpy(auxBufferData->date, toWrite->date);
	auxBufferData->number = toWrite->number;
	strcpy(auxBufferData->message, toWrite->message);

	struct BufferData *auxBufferDataDos = &bufferData[index]; // BORRAR
	printf("PID: %ld\n", auxBufferDataDos->producerID); // BORRAR
	printf("Date: %s\n", strdup(auxBufferDataDos->date)); // BORRAR
	printf("Number: %i\n", auxBufferDataDos->number); // BORRAR
	printf("Message: %s\n", strdup(auxBufferDataDos->message)); // BORRAR

	globalData->lastProduced = (globalData->lastProduced + 1) % 2; // CAMBIAR EL MOD AL TAMAÑO QUE TENGA EL BUFFER;

   	// NO ES NECESARIO USAR memcpy PARA ACTUALIZAR LOS CAMBIOS PORQUE YA SE LE ESTÁ REALIZANDO A LA POSICIÓN DE MEMORIA

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

	int bufferDataDescriptor = shm_open(buffName, O_RDWR, 00200); /* open s.m object*/
	int globalDataDescriptor = shm_open("GlobalData", O_RDWR, 00200);
	struct BufferData *bufferData = loadBufferData(bufferDataDescriptor);
	struct GlobalData *globalData = loadGlobalData(globalDataDescriptor);

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
		writeProcess(bufferData, globalData, produce(buf));
		//end critical region
		sem_post(consumers);
	} // while

	close(bufferDataDescriptor);

	return 0;
} // End of Class
