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
    void setPort(int argC, char **argV, int *portNum, int numArgs);
    void buildHeader(char *buff, char *header, char *filename, int *filesize);
    void serverIntercom(int serverFD, struct sockaddr_in *servAddr);
    void sigintHandler(int sig);
    void initClient(int *clientFD, struct sockaddr_in *servAddr, int portNum);
    void buildRequest(char *file, char *header, int portNum);
    void clientIntercom(int clientFD, struct sockaddr_in *servAddr, int portNum, char *file);

#endif /* HTTP_SHARED_H */

