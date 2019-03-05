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
#define WAIT_SIZE 10

void initServer(int *serverFD, struct sockaddr_in *serverAddr);
void broadcast(int i, int j, int serverFD, int bytesIn, char *inBuff, fd_set *master);
void sendRecv(int i, int serverFD, int maxFD, fd_set *master);
void connectClient(int serverFD, int *maxFD, struct sockaddr_in *clientAddr, fd_set *master);
void showError(char *errorMSG);

int main(void){

    int i = 0;
    int serverFD = 0;
    int maxFD = 0;
    int status = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    fd_set master;
    fd_set temp;

    // Initialize file descriptor lists as empty
    FD_ZERO(&master);
    FD_ZERO(&temp);

    // Initialize Server port file descriptor and address
    initServer(&serverFD, &serverAddr);

    // Add Server 'listening' port to file descriptor master list
    FD_SET(serverFD, &master);

    maxFD = serverFD;
    while(1){

        // Backup master list to expendable temp list
        temp = master;

        // Wait for Client activity
        status = select(maxFD + 1, &temp, NULL, NULL, NULL);
        if(status == -1)
            showError("Failed to select active client.");

        for(i = 0; i <= maxFD; i++){

            if(FD_ISSET(i, &temp)){

                if(i == serverFD)
                    connectClient(serverFD, &maxFD, &clientAddr, &master);
                else
                    sendRecv(i, serverFD, maxFD, &master);

            }

        }

    }

    return 0;

}

void initServer(int *serverFD, struct sockaddr_in *serverAddr){

    int addrLen = 0;
    int status = 0;
    int opt = 1;

    addrLen = sizeof(struct sockaddr_in);

    // Create Server socket file descriptor and address
    *serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*serverFD == -1)
        showError("Failed to create socket.");

    memset(serverAddr, 0, addrLen);
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = INADDR_ANY;
    serverAddr->sin_port = htons(DEF_PORT);

    // Allow the Server socket to be reused
    status = setsockopt(*serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if(status == -1)
        showError("Failed to configure socket.");

    // Bind the Server socket
    status = bind(*serverFD, (struct sockaddr *)serverAddr, addrLen);
    if(status == -1){

        close(*serverFD);
        showError("Failed to bind socket.");

    }

    // Output success message
    printf("\nServer started on port:: %d\n\n", DEF_PORT);
    fflush(stdout);

    // Listen for incoming Client connections
    status = listen(*serverFD, WAIT_SIZE);
    if(status == -1){

        close(*serverFD);
        showError("Failed to connect.");

    }

    return;

}

void broadcast(int i, int j, int serverFD, int bytesIn, char *inBuff, fd_set *master){

    int status = 0;

    if(FD_ISSET(j, master) != 0){

        if((j != serverFD) && (j != i)){

            status = send(j, inBuff, bytesIn, 0);
            if(status == -1)
                fprintf(stderr, "Failed to send message");

        }

    }

    return;

}

void sendRecv(int i, int serverFD, int maxFD, fd_set *master){

    int j = 0;
    int bytesIn = 0;
    char inBuff[BUFFER_SIZE] = {0};

    bytesIn = recv(i, inBuff, BUFFER_SIZE, 0);
    if(bytesIn <= 0){

        if(bytesIn == 0)
            printf("Socket %d disconnected\n", i);
        else
            fprintf(stderr, "\nError receiving message.");

        close(i);
        FD_CLR(i, master);

    }
    else
        for(j = 0; j <= maxFD; j++)
            broadcast(i, j, serverFD, bytesIn, inBuff, master);

    return;

}
void connectClient(int serverFD, int *maxFD, struct sockaddr_in *clientAddr, fd_set *master){

    int addrLen = 0;
    int clientFD = 0;

    addrLen = sizeof(struct sockaddr_in);
    
    clientFD = accept(serverFD, (struct sockaddr *)clientAddr,
        (socklen_t *)&addrLen);
    if(clientFD == -1)
        showError("Failed to accept client connection");
    else{

        FD_SET(clientFD, master);
        if(clientFD > *maxFD)
            *maxFD = clientFD;

        printf("New connection from %s on port %d\n",
            inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));

    }

    return;

}

void showError(char *errorMSG){

    fprintf(stderr, "\n%s\n\n", errorMSG);
    exit(EXIT_FAILURE);

}

