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

char *bufferName;
char *mode;
int avgNumber = 0;
int msgProduced = 0;
long waitTimeSum = 0;

void checkParameters(int argc, char *argv[]){

	//If the user enters -help show the argument sintax	
	if (argc > 1 && strcmp(argv[1], "-help") == 0) {
		puts("Parameters: \n -a = Automatic mode \n -m = Manual mode \n -n \"Buffer name\"\n -t \"AVG Time\"");
		exit(1);
	} // if

	//if there are too few arguments exit
	if (argc < 6) {
		puts("ERROR: Too few arguments. Type -help to get the list of parameters");
		exit(1);
	} // if


	for (int i = 1; i < argc; i++) {
		
		if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "-a") == 0) {
			if (argv[i] != NULL) {
				mode = argv[i];
			} // if
		} // if 

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

void produce(BufferDataPointer bufferData) {
	
	//char *aux = strdup(message); // BORRAR
	//strcpy(bufferData->message, message); // BORRAR
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

void writeProcess(BufferDataPointer bufferData, GlobalDataPointer globalData) {
	
	globalData->lastProduced = (globalData->lastProduced + 1) % globalData->bufferSize;
	int index = globalData->lastProduced; 
	
	BufferData *auxBufferData = &bufferData[index];

	produce(auxBufferData);

	printf(COLOR_YELLOW "====Se Ingresó un mensaje en el Buffer====\n\n" COLOR_RESET
			COLOR_GREEN "PID: " COLOR_RESET "%li\n"
			COLOR_GREEN "Indice del buffer: " COLOR_RESET "%i\n"
			COLOR_GREEN "Consumidores activos: " COLOR_RESET "%i\n"	
			COLOR_GREEN "Productores activos: " COLOR_RESET "%i\n"
			COLOR_YELLOW "==========================================\n\n" COLOR_RESET,
			auxBufferData->producerID,
		    index,
			globalData->activeConsumers,
			globalData->activeProducers);

} // End of writeProcess

int actionMethod (BufferDataPointer bufferData, GlobalDataPointer globalData, sem_t *writeSem) {

	int totalMsgCount; // Listo
	int ProducerTotal; // Listo
	int ConsumersTotal; // Listo
	int deletedByKey; // Listo
	float totalWaitTime; // Listo
	float totalUserTime; // Falta en ambos
	float totalKernelTime; // Falta en ambos

	if(globalData->stateSignal==1) {
		return 0;
	}

	if (strcmp("-a", mode)==0) {
		writeProcess(bufferData, globalData);
		int waitTime = avgNumber;//rand() % (avgNumber * 2);
		sleep(waitTime);
	} else if (strcmp("-m", mode)==0) {
		printf("%s", "Press Enter to generate a message\n");
		getchar();
		writeProcess(bufferData, globalData);
	}
	sem_wait(writeSem);
	globalData->totalMsgCount += 1;
	msgProduced += 1;
	sem_post(writeSem);

	return 1;

} // End of actionMethod

long getTimeSec () {

	time_t timeNow;
	return time(&timeNow);

} // End getTime

int main(int argc, char *argv[]) {

	//Decode arguments
	checkParameters(argc, argv);

	srand(time(NULL)); //Change random number seed

	int bufferDataDescriptor = shm_open(bufferName, O_RDWR, 00200);
	int globalDataDescriptor = shm_open(GLOBAL_DATA_SHM_NAME, O_RDWR, 00200);
	GlobalDataPointer globalData = loadGlobalData(globalDataDescriptor);
	BufferDataPointer bufferData = loadBufferData(bufferDataDescriptor, globalData->bufferSize);	

	//Open semaphores
	sem_t *consumersSem = sem_open(CONSUMER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *producersSem = sem_open(PRODUCER_SEMAPHORE_NAME, O_CREAT, 0644, 3);
	sem_t *writeSem = sem_open(WRITE_SEMAPHORE_NAME, O_CREAT, 0644, 3);


	//Increase the number of active producers in the global counter
	sem_wait(writeSem);
	//begining critical region		
	globalData->activeProducers += 1;
	globalData->producerTotal += 1;
	//end critical region
	sem_post(writeSem);

	while (1) {

		long startTime = getTimeSec();		

		sem_wait(producersSem); // Wait producerSem

		sem_wait(writeSem);	// Wait writeSem
		globalData->totalWaitTime += getTimeSec() - startTime;
		waitTimeSum += getTimeSec() - startTime;
		sem_post(writeSem); // Post writeSem

		if (actionMethod(bufferData, globalData, writeSem) == 0) {
			printf(COLOR_RED "\n====Productor Finalizado====\n\n" COLOR_RESET
					"Mi PID: %i\n"
					"Tiempo de espera: %ld segundos\n"
					"Total de mensajes producidos: %i\n"
				   COLOR_RED "================================\n\n" COLOR_RESET, 
					getpid(),
					waitTimeSum,
					msgProduced
				);
			sem_post(consumersSem);
			break;
		}
		sem_post(consumersSem); // Post consumerSem
	} // while

	sem_wait(writeSem);
	//Decrease the number of active producers on the global counter
	globalData->activeProducers -= 1;
	sem_post(writeSem);

	// Close Files Descriptors
	close(globalDataDescriptor);
	close(bufferDataDescriptor);

	return 0;
} // End of Class
