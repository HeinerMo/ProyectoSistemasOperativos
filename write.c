/*
   It writes to a previously created memory object "/myMemoryObj"
   to be compiled with "-lrt"
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

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

   return ptr;

} // End of writeProcess

int main(void) {

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