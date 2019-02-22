#include "httpShared.h"

int main(int argC, char **argV){

    int serverFD = 0;
    int clientFD = 0;
    int addrLen = 0;
    int portNum = 0;
    int status = 0;
    int filesize = 0;
    int long valRead = 0;
    char buff[BUFFER_SIZE] = {0};
    char header[HEADER_SIZE] = {0};
    char *htmlType = "text/html";
    char *imgType = "image/gif";
    char *data = NULL;
    char *packet = NULL;
    struct sockaddr_in servAddr;
    FILE *infile = NULL;

    addrLen = sizeof(servAddr);

    // Verify user input
    if(argC < 3){

        if(argC == 2){

            portNum = atoi(argV[1]);
            if((portNum < 60000) || (portNum > 60099)){

                printf("\n*Specify a port number between 60000 and 60099.\n\n");
                exit(EXIT_FAILURE);

            }

        }
        else
            portNum = DEF_PORT;

        // Create Server socket file descriptor
        serverFD = socket(AF_INET, SOCK_STREAM, 0);
        if(serverFD == 0){

            fprintf(stderr, "Failed to create socket.");
            exit(EXIT_FAILURE);

        }

        memset(servAddr.sin_zero, 0, sizeof(servAddr.sin_zero));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = INADDR_ANY;
        servAddr.sin_port = htons(portNum);

        // Bind the Server socket
        status = bind(serverFD, (struct sockaddr *)&servAddr, addrLen);
        if(status < 0){

            fprintf(stderr, "Failed to bind socket.");
            close(serverFD);
            exit(EXIT_FAILURE);

        }

        // Listen for incoming Client connections
        status = listen(serverFD, WAIT_SIZE);
        if(status < 0){

            fprintf(stderr, "Failed to connect.");
            close(serverFD);
            exit(EXIT_FAILURE);

        }

        // Accept Client connections
        while(1){

            printf("\n++++++++++ Waiting for new connection ++++++++++\n\n");
            clientFD = accept(serverFD, (struct sockaddr *)&servAddr,
                (socklen_t *)&addrLen);
            if(clientFD < 0){

                fprintf(stderr, "Connection terminated unexpectedly.");
                close(serverFD);
                exit(EXIT_FAILURE);

            }

            // Receive message from Client
            valRead = read(clientFD, buff, BUFFER_SIZE);
            if(valRead > 0){

                printf("%s\n\n", buff);

                infile = fopen("index.html", "rb");
                fseek(infile, 0, SEEK_END);
                filesize = ftell(infile);
                fseek(infile, 0, SEEK_SET);
                data = malloc(filesize * sizeof(char) + 1);
                fread(data, filesize, 1, infile);

                // Send message to Client
                sprintf(header, "HTTP/1.1 200 OK\nContent-Type: %s"
                    "\nContent-Length: %d\n\n", htmlType, filesize);
                packet = malloc(strlen(header) + filesize + 1);
                strcpy(packet, header);
                strcat(packet, data);
                write(clientFD, packet, strlen(packet));
                memset(header, 0, HEADER_SIZE);
                free(data);
                free(packet);
                fclose(infile);

            }
            else if(valRead == 0)
                fprintf(stderr, "Client message was empty.");
            else
                fprintf(stderr, "Error reading client message.");

            memset(buff, 0, BUFFER_SIZE);
            close(clientFD);

        }

    }
    else
        printf("\nProper usage:\n\n"
            "./server <port>\n\n"
            "\t*Ensure that <port> is between 60000 and 60099.\n\n"
            "---------- OR ----------\n\n"
            "./server\n\n"
            "\t*Server port will default to 60032.\n\n");

    return 0;

}

