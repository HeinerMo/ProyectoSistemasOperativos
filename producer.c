//Producer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
 


int main(int argc, char *argv[]) 
{


   char *buffname=argv[1]; //bufferName
   int fd;
   char buf[] = "Hello,this is writting process";
   char *ptr;
   
   fd = shm_open (buffname,  O_RDWR  , 00200); /* open s.m object*/
   if(fd == -1)
   {
       printf("Error file descriptor %s\n", strerror(errno));
	   exit(1);
   }
   
   ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
   if(ptr == MAP_FAILED)
   {
      printf("Map failed in write process: %s\n", strerror(errno));
      exit(1);
   }
   
   memcpy(ptr,buf, sizeof(buf));
   printf("%d \n", (int)sizeof(buf));
   close(fd);
   
   return 0;
}
