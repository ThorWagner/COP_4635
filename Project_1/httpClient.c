#include <arpa/inet.h>
#include "httpShared.h"

void initClient(int *clientFD, struct sockaddr_in *servAddr, int portNum);
void buildRequest(char *file, char *header, int portNum);
void clientIntercom(int clientFD, struct sockaddr_in *servAddr, int portNum, char *file);

int main(int argC, char **argV){

    int clientFD = 0;
//    int addrLen = 0;
    int portNum = 0;
    char opt = 'y';
    char file[ARG_SIZE] = {0};
//    int status = 0;
//    int long valRead = 0;
//    char buff[BUFFER_SIZE] = {0};
//    char header[HEADER_SIZE] = {0};
//    char *request = "Hello.";
    struct sockaddr_in servAddr;

//    addrLen = sizeof(servAddr);
    
    // Verify user input
    if((argC > 1) && (argC < 4)){

        // Set port number according to user input or default
        setPort(argC, argV, &portNum, 3);

        // Extract filename from user input
        if(argC == 2)
            strcpy(file, argV[1]);
        else
            strcpy(file, argV[2]);

        // Initialize Server port file descriptor and address
        initClient(&clientFD, &servAddr, portNum);

        while(1){

            clientIntercom(clientFD, &servAddr, portNum, file);
            
            memset(file, 0, ARG_SIZE);
            printf("Would you like to make another request? [y/n]: ");
            scanf("\n\n%c", &opt);
            if((opt == 'n') || (opt == 'N'))
                break;
            printf("Enter the file to request: ");
            scanf("\n\n%s", file);

        }

    }
    else
        printf("\nProper usage:\n\n"
            "./client [port] [file]\n\n"
            "\t*Ensure that [port] is between 60000 and 60099.\n\n"
            "---------- OR ----------\n\n"
            "./client <message>\n\n"
            "\t*Server port will default to 60032.\n\n");

    close(clientFD);

    return 0;

}

void initClient(int *clientFD, struct sockaddr_in *servAddr, int portNum){

    int addrLen = 0;
    int status = 0;

    addrLen = sizeof(servAddr);

    // Create Server socket file descriptor
    *clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if(clientFD < 0){

        fprintf(stderr, "Failed to create socket.");
        exit(EXIT_FAILURE);

    }

    memset(servAddr, 0, addrLen);
    servAddr->sin_family = AF_INET;
    servAddr->sin_port = htons(portNum);

    // Convert IPv4 and IPv6 addresses from text to binary form
    status = inet_pton(AF_INET, "127.0.0.1", &servAddr->sin_addr);
    if(status <= 0){

        fprintf(stderr, "Invalid address.");
        close(*clientFD);
        exit(EXIT_FAILURE);

    }

    return;

}

void buildRequest(char *file, char *header, int portNum){

    char type[ARG_SIZE] = {0};
    char *request = "GET /%s HTTP/1.1\r\n"
                    "Host: localhost:%d\r\n"
                    "Connection: keep-alive\r\n"
                    "Content-Length: 0\r\n"
                    "Content-Type: %s\r\n\r\n";

    // Check for and set common file types
    if(strstr(file, ".htm") != NULL)
        strcpy(type, "text/html");
    else if(strstr(file, ".ico") != NULL)
        strcpy(type, "image/vnd.microsoft.icon");
    else if(strstr(file, ".gif") != NULL)
        strcpy(type, "image/gif");
    else if(strstr(file, ".png") != NULL)
        strcpy(type, "image/png");
    else if(strstr(file, ".jp") != NULL)
        strcpy(type, "image/jpeg");
    else
        strcpy(type, "unknown");

    // Construct request header
    sprintf(header, request, file, portNum, type);

    return;

}

void clientIntercom(int clientFD, struct sockaddr_in *servAddr, int portNum, char *file){

    int status = 0;
    int addrLen = 0;
    int long valRead = 0;
    char buff[BUFFER_SIZE] = {0};
    char header[HEADER_SIZE] = {0};

    addrLen = sizeof(struct sockaddr_in);

    // Construct request header based on user input
    buildRequest(file, header, portNum);

    // Connect to Server
    printf("\n++++++++ Attempting to connect ++++++++\n\n");
    status = connect(clientFD, (struct sockaddr *)servAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "Failed to connect to server.");
        close(clientFD);
        exit(EXIT_FAILURE);

    }
    
    // Send request to Server
    send(clientFD, header, strlen(header), 0);
    printf("------------ Request sent ------------\n\n%s\n\n", header);

    // Receive response from Server
    valRead = recv(clientFD, buff, BUFFER_SIZE, 0);
    if(valRead > 0){

        printf("---------- Response received ----------\n\n%s", buff);
//        memset(buff, 0, BUFFER_SIZE);
//        valRead = recv(clientFD, buff, BUFFER_SIZE, 0);
        printf("%ld bytes recieved.\n\n", valRead);

    }
    else if(valRead == 0)
        fprintf(stderr, "Server message was empty.");
    else
        fprintf(stderr, "Error reading server message.");

    return;

}

