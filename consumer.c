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

int msgConsumed = 0;
long waitTimeSum = 0;

char *bufferName;
int avgNumber = 0;
long userTime = 0;

void checkParameters(int argc, char *argv[]){

	//If the user enters -help show the argument sintax	
	if (argc > 1 && strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -n \"Buffer name\"\n -t \"AVG Time\"");
		exit(1);
	} // if

	//if there are too few arguments exit
	if (argc < 5) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		exit(1);
	} // if


	for (int i = 1; i < argc; i++) {
		
		if (strcmp(argv[i], "-t") == 0) {
			if (argv[i+1] != NULL) {
				avgNumber = strtol(argv[i+1], NULL, 10);
			} // if
		} // if 

		if (strcmp(argv[i], "-n") == 0) {
			if (argv[i+1] != NULL) {
				bufferName = argv[i+1];
			} // if
		} // if

	} // for i

} // Fin de checkParameters

GlobalData *loadGlobalData(int globalDataDescriptor) {

   	if (globalDataDescriptor == -1) {
    	printf("No existe esa memoria compartida");
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
    	printf("No existe esa memoria compartida");
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

	sprintf(data, COLOR_CYAN "====Se recibió un mensaje del el Buffer====\n\n" COLOR_RESET
					COLOR_GREEN "ID Productor: " COLOR_RESET "%ld\n"
					COLOR_GREEN "Hora y Fecha: " COLOR_RESET "%s\n"
					COLOR_GREEN "Número mágico: " COLOR_RESET "%d \n"
					COLOR_GREEN "Consumidores activos: " COLOR_RESET "%i\n"	
					COLOR_GREEN "Productores activos: " COLOR_RESET "%i\n"
				  COLOR_CYAN "===========================================\n\n" COLOR_RESET,
		       		auxBufferData->producerID, 
					auxBufferData->date, 
					auxBufferData->number, 
					globalData->activeConsumers,
					globalData->activeProducers);

	if ((getpid()%10) == auxBufferData->number) {
		globalData->deletedByKey += 1;
		return "Número Mágico";
	} else if (globalData->stateSignal==1) {
		return "Por el Finalizador";
	}

	char *bufferMessage = strdup(data);
	return bufferMessage;

} // End of consume

long getTimeSec () {

	time_t timeNow;
	return time(&timeNow);

} // End getTime


int main(int argc, char *argv[]) {

	//Decode arguments
	checkParameters(argc, argv);

	userTime = getTimeSec();

	int bufferDataDescriptor = shm_open(bufferName, O_RDWR, 00200); 
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
		waitTimeSum += getTimeSec() - startTime;
		sem_post(writeSem); // Post writeSem

		char* bufferMessage = consume(globalData, bufferData, writeSem);
		/*muestra su identificaci´on,
		raz´on de suspensi´on, y algunas estad´ısticas b´asicas (n´umero de mensajes consumidos, acumulado
		de tiempos esperados, acumulado de tiempo que estuvo bloqueado, tiempo de usuario . . . ).*/
		if (strcmp(bufferMessage, "Número Mágico") == 0 || strcmp(bufferMessage, "Por el Finalizador") == 0) {
			userTime = getTimeSec() - userTime;
			printf("\n" COLOR_RED "====Consumidor Finalizado====" COLOR_RESET "\n\n"
					"Mi PID: %i\n"
					"Finalizado por: %s\n"
					"Total de mensajes consumidos: %i segundos\n"
					"Tiempo en espera: %ld segundos\n"
					"Tiempo de usuario: %ld segundos\n"
				   		COLOR_RED "=============================" COLOR_RESET "\n\n",
					getpid(),
					bufferMessage,
					msgConsumed,
					waitTimeSum,
					userTime
				);
			sem_post(producersSem);
			break;
		} else {
			msgConsumed += 1;
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