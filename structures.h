#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define PRODUCER_SEMAPHORE_NAME "producerSemaphore"
#define CONSUMER_SEMAPHORE_NAME "consumerSemaphore"
#define WRITE_SEMAPHORE_NAME "writeSemaphore"
#define GLOBAL_DATA_SHM_NAME "GlobalData"

// Colores
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

struct BufferDataStruct {
	long producerID;
	char date[80];
	int number;
	//char message[1024];
};

typedef struct BufferDataStruct BufferData; // Uso de la estructura del BufferData
typedef BufferData *BufferDataPointer; /* Puntero("Array") de BufferData. Puede ser visto como
										 una variable global*/

struct GlobalDataStruct {
	int lastConsumed;
	int lastProduced;
	int activeProducers;
	int activeConsumers;
	int bufferSize;
	int stateSignal; // 1 = Finalizar o 0 = Ejecutando
	int totalMsgCount;
	int producerTotal;
	int consumersTotal;
	int deletedByKey;
	long totalWaitTime;
	long totalBlockedTime;
	long totalUserTime;
};

typedef struct GlobalDataStruct GlobalData; // Uso de la estructura del GlobalData
typedef GlobalData *GlobalDataPointer; /* Puntero de GlobalData. Puede ser visto como
										 una variable global*/

#endif