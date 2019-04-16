#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define DEF_PORT 8080
#define WAIT_SIZE 10
#define PARAM_SIZE 1024
#define BUFFER_SIZE 4096
#define NEW 1

void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum);
void threadedHandler(void *serverFD);

int main(int argC, char **argV){

    int serverFD = 0;
    int clientFD = 0;
    int addrLen = 0;
    int *newSocket = NULL;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    addrLen = sizeof(struct sockaddr_in);
    newSocket = malloc(sizeof(int));

    initServer(&serverFD, &serverAddr, DEF_PORT);

    status = listen(serverFD, WAIT_SIZE);
    if(status < 0){

        fprintf(stderr, "\nFailed to connect.\n\n\n");
        close(serverFD);
        return 1;

    }

    while(clientFD = accept(serverFD, (struct sockaddr *)&clientAddr,
        (socklen_t *)&addrLen)){

        pthread_t newConnect;
        *newSocket = ClientFD;
        pthread_create(&newConnect, NULL, threadedHandler, (void *)newSocket);
        pthread_join(newConnect, NULL);

    }

    return 0;

}

void initServer(int *serverFD, struct sockaddr_in *serverAddr, int portNum){

    int status = 0;
    int addrLen = 0;

    addrLen = sizeof(struct sockaddr_in);

    // Create the Server socket file descriptor
    *serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*serverFD < 0){

        fprintf(stderr, "\nFailed to create socket.\n\n\n");
        exit(EXIT_FAILURE);

    }

    memset(serverAddr, 0, addrLen);
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = INADDR_ANY;
    serverAddr->sin_port = htons(portNum);

    // Bind the Server socket
    status = bind(*serverFD, (struct sockaddr *)servAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "\nFailed to bind socket.\n\n\n");
        close(*serverFD);
        exit(EXIT_FAILURE);

    }

    return;

}

void threadedHandler(void *serverFD){

    int socket = 0;
    int valRead = 0;
    char filename[PARAM_SIZE] = {0};
    char msg[BUFFER_SIZE] = {0};

    socket = *(int *)serverFD;

    valRead = recv(socket, msg, BUFFER_SIZE, 0);
    if(valRead > 0){

        strcpy(filename, parseFilename(msg));

    }

    return;

}

