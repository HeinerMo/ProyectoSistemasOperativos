/*
   It reads from a previously created memory object "/myMemoryObj"
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
    } // for i

} // End of endLine

char* readProcess (int fd) {

   char *ptr;
    struct stat shmobj_st;

    if (fd == -1)
    {
        printf("Error file descriptor %s\n", strerror(errno));
        exit(1);
    }

    if (fstat(fd, &shmobj_st) == -1)
    {
        printf(" error fstat \n");
        exit(1);
    }
    ptr = mmap(NULL, shmobj_st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed in read process: %s\n", strerror(errno));
        exit(1);
    }

    return ptr;

} // End of readProcess

int main(void) {

   int lenght = 2048;
   char buffName[lenght];
   printf("%s", "Enter the name of the buffer: ");
   fgets(buffName, lenght, stdin);
   endLine(buffName, lenght);
 
    int fd = shm_open(buffName, O_RDONLY, 00400); /* open s.m object*/
   
    while (1) {

        char* ptr = readProcess(fd);
        printf("%s \n", ptr);

        sleep(2);

    } // while
    
    close(fd);

    return 0;

} // Fin de main