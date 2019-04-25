#include "ftpClient.h"

void initClient(int *clientFD, struct sockaddr_in *serverAddr, int portNum);

int main(int argC, char **argV){

    int clientFD = 0;
    int valRead = 0;
    int size = 0;
    int newFD = 0;
    char *data = NULL;
    char msg[BUFFER_SIZE] = {0};
    struct sockaddr_in serverAddr;

    initClient(&clientFD, &serverAddr, DEF_PORT);

    // Request file from Server
    strcpy(msg, "GET ");
    strcat(msg, "test.txt");
    send(clientFD, msg, strlen(msg), 0);

    // Get Server response
    memset(msg, 0, BUFFER_SIZE);
    valRead = recv(clientFD, msg, BUFFER_SIZE, 0);

    if(valRead > 0){

        if(strcmp(msg, "OK") == 0){

            recv(clientFD, &size, sizeof(int), 0);
            data = malloc(size);
            newFD = open("test.txt", O_CREAT | O_EXCL | O_WRONLY, 0666);
            valRead = recv(clientFD, data, size, 0);
            if(valRead > 0){


                printf("\nFile received.\n\n");
                write(newFD, data, size);
                free(data);
                close(newFD);

            }
            else
                printf("Error line 34");

        }
        else
            printf("Error line 28");

    }
    else
        fprintf(stderr, "Request could not be completed.");

    return 0;

}

void initClient(int *clientFD, struct sockaddr_in *serverAddr, int portNum){

    int addrLen = 0;
    int status = 0;

    addrLen = sizeof(serverAddr);

    // Create Client socket file descriptor
    *clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*clientFD == -1){

        fprintf(stderr, "Failed to create socket.");
        exit(EXIT_FAILURE);

    }

    memset(serverAddr, 0, addrLen);
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(portNum);

    // Convert IPv4 and IPv6 addresses from text to binary form
    status = inet_pton(AF_INET, "127.0.0.1", &serverAddr->sin_addr);
    if(status <= 0){

        fprintf(stderr, "Invalid address.");
        close(*clientFD);
        exit(EXIT_FAILURE);

    }

    return;

}

