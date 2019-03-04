#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define DEF_PORT 60032
#define BUFFER_SIZE 1024

void initServer(int *socketFD, struct sockaddr_in *serverAddr);
void broadcast(int i, int j, int socketFD, int bytesIn, char *inBuff, fd_set *master);
void sendRecv(int i, int socketFD, int maxFD, fd_set *master);
void connectClients(int socketFD, int *maxFD, struct sockaddr_in *clientAddr, fd_set *master);

int main(void){

    int i = 0;
    int socketFD = 0;
    int maxFD = 0;
    int status = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    fd_set master;
    fd_set listFD;

    FD_ZERO(&master);
    FD_ZERO(&listFD);
    initServer(&socketFD, &serverAddr);
    FD_SET(socketFD, &master);

    maxFD = socketFD;
    while(1){

        listFD = master;

        status = select(maxFD + 1, &listFD, NULL, NULL, NULL);
        if(status == -1){

            perror("Select");
            exit(EXIT_FAILURE);

        }

        for(i = 0; i <= maxFD; i++){

            if(FD_ISSET(i, &listFD)){

                if(i == socketFD)
                    connectClients(socketFD, &maxFD, &clientAddr, &master);
                else
                    sendRecv(i, socketFD, maxFD, &master);

            }

        }

    }

    return 0;

}

void initServer(int *socketFD, struct sockaddr_in *serverAddr){

    int status = 0;
    int opt = 1;

    *socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketFD == -1){

        perror("Socket");
        exit(EXIT_FAILURE);

    }

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(DEF_PORT);
    serverAddr->sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr->sin_zero, 0, sizeof(serverAddr->sin_zero));

    status = setsockopt(*socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if(status == -1){

        perror("setsockopt()");
        exit(EXIT_FAILURE);

    }

    status = bind(*socketFD, (struct sockaddr *)serverAddr, sizeof(struct sockaddr));
    if(status == -1){

        perror("Bind");
        exit(EXIT_FAILURE);

    }

    status = listen(*socketFD, 10);
    if(status == -1){

        perror("Listen");
        exit(EXIT_FAILURE);

    }

    printf("\nWaiting for clients on port %d\n", DEF_PORT);
    fflush(stdout);

    return;

}

void broadcast(int i, int j, int socketFD, int bytesIn, char *inBuff, fd_set *master){

    int status = 0;

    if(FD_ISSET(j, master) != 0){

        if((j != socketFD) && (j != i)){

            status = send(j, inBuff, bytesIn, 0);
            if(status == -1)
                perror("Send Message");

        }

    }

    return;

}

void sendRecv(int i, int socketFD, int maxFD, fd_set *master){

    int j = 0;
    int bytesIn = 0;
    char inBuff[BUFFER_SIZE] = {0};

    bytesIn = recv(i, inBuff, BUFFER_SIZE, 0);
    if(bytesIn <= 0){

        if(bytesIn == 0)
            printf("Socket %d disconnected\n", i);
        else
            perror("Receive Message");

        close(i);
        FD_CLR(i, master);

    }
    else
        for(j = 0; j <= maxFD; j++)
            broadcast(i, j, socketFD, bytesIn, inBuff, master);

    return;

}
void connectClients(int socketFD, int *maxFD, struct sockaddr_in *clientAddr, fd_set *master){

    int addrLen = 0;
    int clientFD = 0;

    addrLen = sizeof(struct sockaddr_in);
    
    clientFD = accept(socketFD, (struct sockaddr *)clientAddr,
        (socklen_t *)&addrLen);
    if(clientFD == -1){

        perror("Accept");
        exit(EXIT_FAILURE);

    }
    else{

        FD_SET(clientFD, master);
        if(clientFD > *maxFD)
            *maxFD = clientFD;

        printf("New connection from %s on port %d\n",
            inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));

    }

    return;

}

