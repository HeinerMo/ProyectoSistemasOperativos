//Producer.c 
//Authors: 
//	Heiner Monge.
//	Antoni Seas.
//	Susan Sojo
//Date: November 2021

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
<<<<<<< HEAD
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
=======
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
<<<<<<< HEAD
 


int main(int argc, char *argv[]) 
{
=======

void endLine(char *message, int length) {

   for (int i = 0; i < length; i++) {
      if (message[i] == '\n') {
         message[i] = '\0';
         break;
      } // if
   }    // for i

} // Fin de endLine

char *writeProcess(int fd, char *buf) {

   char *ptr;

   if (fd == -1) {
      printf("Error file descriptor %s\n", strerror(errno));
      exit(1);
   }

   ptr = mmap(NULL, sizeof(buf), PROT_WRITE, MAP_SHARED, fd, 0);
   if (ptr == MAP_FAILED) {
      printf("Map failed in write process: %s\n", strerror(errno));
      exit(1);
   }

   memcpy(ptr, buf, sizeof(buf));
>>>>>>> 70a33a550a1f6dbe9975337dc95e6f5c525649a1

   return ptr;

} // End of writeProcess

<<<<<<< HEAD
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
=======
int main(int argc, char *argv[]) {
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

      	printf("%d \n", (int)sizeof(buf));

   } // while

   	close(fd);

   	return 0;
} // End of Class
>>>>>>> 70a33a550a1f6dbe9975337dc95e6f5c525649a1
