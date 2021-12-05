//Finisher.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>

#include "structures.h"

void endLine(char *message, int length) { //FIXME Borrar

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	} // for i

} // End of endLine


int main(int argc, char *argv[]) {
	
	int lenght = 2048;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	char instruction[lenght];
	sprintf(instruction, "rm /dev/shm/%s", buffName);
	system(instruction);
	sprintf(instruction, "rm /dev/shm/%s", GLOBAL_DATA_SHM_NAME);
	system(instruction);
	sprintf(instruction, "rm /dev/shm/sem.%s", CONSUMER_SEMAPHORE_NAME);
	system(instruction);
	sprintf(instruction, "rm /dev/shm/sem.%s", PRODUCER_SEMAPHORE_NAME);
	system(instruction);
	sprintf(instruction, "rm /dev/shm/sem.%s", WRITE_SEMAPHORE_NAME);
	system(instruction);

	// Restar el tiempo actual a totalUserTime
	return 0;
} // End of main
