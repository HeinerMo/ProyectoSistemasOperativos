#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__

#define PRODUCER_SEMAPHORE_NAME "producerSemaphore"
#define CONSUMER_SEMAPHORE_NAME "consumerSemaphore"
#define GLOBAL_DATA_SHM_NAME "GlobalData"

struct BufferDataStruct {
	long producerID;
	char date[80];
	int number;
	char message[1024];
};

typedef struct BufferDataStruct BufferData; // Uso de la estructura del BufferData
typedef BufferData *BufferDataPointer; /* Puntero("Array") de BufferData. Puede ser visto como
										 una variable global*/

struct GlobalDataStruct {
	int lastConsumed;
	int lastProduced;
	int activeProducers;
	int activeConsumers;
	int produce;
	int bufferSize;
};

typedef struct GlobalDataStruct GlobalData; // Uso de la estructura del GlobalData
typedef GlobalData *GlobalDataPointer; /* Puntero de GlobalData. Puede ser visto como
										 una variable global*/

#endif