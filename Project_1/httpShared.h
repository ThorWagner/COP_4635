#ifndef HTTP_SHARED_H
#define HTTP_SHARED_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    
    #define BUFFER_SIZE 1024
    #define HEADER_SIZE 512
    #define ARG_SIZE 100
    #define BLOCK_SIZE 1024
    #define DEF_PORT 60032
    #define WAIT_SIZE 10
    #define MAX_FILESIZE 100000

    void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum);
    void setPort(int argC, char **argV, int *portNum);
    void buildHeader(char *buff, char *header, char *filename, int *filesize);
    void serverIntercom(int serverFD, struct sockaddr_in *servAddr);
    void sigintHandler(int sig);

#endif /* HTTP_SHARED_H */

