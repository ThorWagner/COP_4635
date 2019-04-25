#include "ftpServer.h"

void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum);
void *threadedHandler(void *serverFD);
void parseFilename(char *msg, char *filename);
bool transmitFile(int serverFD, char *filename);
void sigintHandler(int sig);

int main(int argC, char **argV){

    int serverFD = 0;
    int clientFD = 0;
    int addrLen = 0;
    int status = 0;
    int *newSocket = NULL;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    addrLen = sizeof(struct sockaddr_in);
    newSocket = malloc(sizeof(int));

    initServer(&serverFD, &serverAddr, DEF_PORT);

    // Handle keyboard interrupt [^C]
    signal(SIGINT, sigintHandler);

    status = listen(serverFD, WAIT_SIZE);
    if(status < 0){

        fprintf(stderr, "\nFailed to connect.\n\n\n");
        close(serverFD);
        return 1;

    }

    while((clientFD = accept(serverFD, (struct sockaddr *)&clientAddr,
    (socklen_t *)&addrLen))){

        pthread_t newConnect;
        *newSocket = clientFD;
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
    status = bind(*serverFD, (struct sockaddr *)serverAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "\nFailed to bind socket.\n\n\n");
        close(*serverFD);
        exit(EXIT_FAILURE);

    }

    return;

}

void *threadedHandler(void *serverFD){

    int socket = 0;
    int valRead = 0;
    char filename[PARAM_SIZE] = {0};
    char msg[BUFFER_SIZE] = {0};

    socket = *(int *)serverFD;

    valRead = recv(socket, msg, BUFFER_SIZE, 0);
    if(valRead > 0){

        parseFilename(msg, filename);
        memset(msg, 0, BUFFER_SIZE);

        // Check if file exists
        if(access(filename, F_OK) != -1){

            strcpy(msg, "OK");
            send(socket, msg, strlen(msg), 0);
            transmitFile(socket, filename);

        }
        else{

            strcpy(msg, "NO");
            send(socket, msg, strlen(msg), 0);

        }

    }

    close(socket);

    return NULL;

}

void parseFilename(char *msg, char *filename){

    char *temp = NULL;

    temp = strchr(msg, ' ');
    strcpy(filename, temp + 1);

    return;

}

bool transmitFile(int serverFD, char *filename){

    int newFD = 0;
    int size = 0;
    bool sent = false;
    struct stat file;

    stat(filename, &file);
    newFD = open(filename, O_RDONLY);
    size = file.st_size;
    send(serverFD, &size, sizeof(int), 0);
    if(sendfile(serverFD, newFD, NULL, size) != -1)
        sent = true;

    return sent;

}

void sigintHandler(int sig){

    // Verify server process is ending
    fprintf(stderr, "\nShutting down server...\n\n\n");

    // Return SIGINT
    exit(sig);
    
    return;

}

