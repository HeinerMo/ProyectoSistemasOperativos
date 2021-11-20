//Producer.c 
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

struct BufferData {
	char producerID[50];
	char date[20];
	char time[20];
	int number;
};

void endLine(char *message, int length) {

	for (int i = 0; i < length; i++) {
		if (message[i] == '\n') {
			message[i] = '\0';
			break;
		} // if
	}    // for i

} // Fin de endLine

char *writeProcess(int fd, char *buf) {

	struct BufferData *buffer;

	if (fd == -1) {
		printf("Error file descriptor %s\n", strerror(errno));
		exit(1);
	}

	buffer = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
	if (buffer == MAP_FAILED) {
		printf("Map failed in write process: %s\n", strerror(errno));
		exit(1);
	}

	memcpy(buffer->producerID, buf, sizeof(buf));

	return buffer->producerID;

} // End of writeProcess

int main(int argc, char *argv[]) {
	/*TODO
	 	Abrir el buffer
		Abrir los dos semáforos
		Producir
		Establecer la región crítica
		Recibir nombre por parámetros
	 */
	int lenght = 2048;
	char buffName[lenght];
	printf("%s", "Enter the name of the buffer: ");
	fgets(buffName, lenght, stdin);
	endLine(buffName, lenght);

	int fd = shm_open(buffName, O_RDWR, 00200); /* open s.m object*/




	while (1) {
		char buf[lenght];
		printf("%s", "Enter some text: ");
		fgets(buf, lenght, stdin);
		endLine(buf, lenght);
		char *ptr = writeProcess(fd, buf);
		printf("%s \n",ptr);
	} // while

	close(fd);

	return 0;
} // End of Class
