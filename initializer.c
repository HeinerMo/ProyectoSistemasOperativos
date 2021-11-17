//Initializer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	puts("Initializer.c running...");
	pid_t pid = getpid();

	printf("pid: %lu", pid);//prints PID

	return 0;
}
