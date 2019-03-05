#include "chatShared.h"

void initClient(int *socketFD, struct sockaddr_in *serverAddr);
void sendRecv(int i, int socketFD);

int main(void){

    int i = 0;
    int socketFD = 0;
    int maxFD = 0;
    int current = 0;
    struct sockaddr_in serverAddr;
    fd_set master;
    fd_set listFD;

    initClient(&socketFD, &serverAddr);
    FD_ZERO(&master);
    FD_ZERO(&listFD);
    FD_SET(0, &master);
    FD_SET(socketFD, &master);
    maxFD = socketFD;

    while(1){

        listFD = master;
        current = select(maxFD + 1, &listFD, NULL, NULL, NULL);
        if(current == -1){

            perror("Select");
            exit(EXIT_FAILURE);

        }

        for(i = 0; i <= maxFD; i++)
            if(FD_ISSET(i, &listFD) != 0)
                sendRecv(i, socketFD);

    }

    printf("Exiting...\n\n");
    close(socketFD);

    return 0;

}

void initClient(int *socketFD, struct sockaddr_in *serverAddr){

    int status = 0;
    
    *socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketFD == -1){

        perror("Socket");
        exit(EXIT_FAILURE);

    }

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(DEF_PORT);
    serverAddr->sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr->sin_zero, 0, sizeof(serverAddr->sin_zero));

    status = connect(*socketFD, (struct sockaddr *)serverAddr,
        sizeof(struct sockaddr));
    if(status == -1){

        perror("Connection");
        exit(EXIT_FAILURE);

    }

    return;

}

void sendRecv(int i, int socketFD){

    char inBuff[BUFFER_SIZE] = {0};
    char outBuff[BUFFER_SIZE] = {0};
//    int bytesIn = 0;

    if(i == 0){

        scanf("\n\n%[^\n]", outBuff);
        if(strcmp(outBuff, "exit") == 0)
            exit(EXIT_SUCCESS);
        else
            send(socketFD, outBuff, strlen(outBuff), 0);

    }
    else{

        recv(socketFD, inBuff, BUFFER_SIZE, 0);
        printf("%s\n", inBuff);
        fflush(stdout);

    }

    return;

}

