#include <arpa/inet.h>
#include "httpShared.h"

int main(int argC, char **argV){

    int clientFD = 0;
    int addrLen = 0;
    int portNum = 0;
    int status = 0;
    int long valRead = 0;
    char buff[BUFFER_SIZE] = {0};
    char *request = "Hello.";
    struct sockaddr_in servAddr;

    addrLen = sizeof(servAddr);

    // Verify user input
    if((argC > 1) && (argC < 4)){

        if(argC == 3){

            portNum = atoi(argV[2]);
            if((portNum < 60000) || (portNum > 60099)){

                printf("\n*Specify a port number between 60000 and 60099.\n\n");
                exit(EXIT_FAILURE);

            }

        }
        else
            portNum = DEF_PORT;

        // Create Client socket file descriptor
        clientFD = socket(AF_INET, SOCK_STREAM, 0);
        if(clientFD < 0){

            fprintf(stderr, "Failed to create socket.");
            exit(EXIT_FAILURE);

        }

        memset(&servAddr, 0, addrLen);
        servAddr.sin_family = AF_INET;
        servAddr.sin_port = htons(portNum);

        // Convert IPv4 and IPv6 addresses from text to binary form
        status = inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);
        if(status <= 0){

            fprintf(stderr, "Invalid address.");
            close(clientFD);
            exit(EXIT_FAILURE);

        }

        // Connect to Server
        printf("\n++++++++ Attempting to connect ++++++++\n\n");
        status = connect(clientFD, (struct sockaddr *)&servAddr, addrLen);
        if(status < 0){

            fprintf(stderr, "Failed to connect to server.");
            close(clientFD);
            exit(EXIT_FAILURE);

        }

        // Send request to Server
        send(clientFD, request, strlen(request), 0);
        printf("------------ Request sent ------------\n\n");

        // Receive response from Server
        valRead = recv(clientFD, buff, BUFFER_SIZE, 0);
        if(valRead > 0)
            printf("%s\n\n", buff);
        else if(valRead == 0)
            fprintf(stderr, "Server message was empty.");
        else
            fprintf(stderr, "Error reading server message.");

    }
    else
        printf("\nProper usage:\n\n"
            "./client <port> <message>\n\n"
            "\t*Ensure that <port is between 60000 and 60099.\n\n"
            "---------- OR ----------\n\n"
            "./client <message>\n\n"
            "\t*Server port will default to 60032.\n\n");

    close(clientFD);

    return 0;

}

