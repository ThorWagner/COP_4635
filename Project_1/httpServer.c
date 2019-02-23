#include <signal.h>
#include "httpShared.h"

void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum);
void setPort(int argC, char **argV, int *portNum);
void buildResponse(char *buff, char **packet);
void serverIntercom(int serverFD, struct sockaddr_in *servAddr);
void sigintHandler(int sig);

int main(int argC, char **argV){

    int serverFD = 0;
    int portNum = 0;
    int status = 0;
    struct sockaddr_in servAddr;

    // Verify user input
    if(argC < 3){

        // Set port number according to user input or default
        setPort(argC, argV, &portNum);

        // Initialize Server port file descriptor and address
        initServer(&serverFD, &servAddr, portNum);

        // Listen for incoming Client connections
        status = listen(serverFD, WAIT_SIZE);
        if(status < 0){

            fprintf(stderr, "Failed to connect.\n\n");
            close(serverFD);
            exit(EXIT_FAILURE);

        }

        // Handle signal interrupt [^C]
        signal(SIGINT, sigintHandler);

        // Accept Client connections and respond
        while(1){

            serverIntercom(serverFD, &servAddr);

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

void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum){

    int addrLen = 0;
    int status = 0;

    addrLen = sizeof(struct sockaddr_in);
    
    // Create Server socket file descriptor
    *serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*serverFD == 0){

        fprintf(stderr, "\nFailed to create socket.\n\n");
        exit(EXIT_FAILURE);

    }

    memset(servAddr, 0, addrLen);
    servAddr->sin_family = AF_INET;
    servAddr->sin_addr.s_addr = INADDR_ANY;
    servAddr->sin_port = htons(portNum);

    // Bind the Server socket
    status = bind(*serverFD, (struct sockaddr *)servAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "\nFailed to bind socket.\n\n");
        close(*serverFD);
        exit(EXIT_FAILURE);

    }
    
    // Output success message
    printf("\nServer started on port: %d\n\n", portNum);

    return;

}

void setPort(int argC, char **argV, int *portNum){

    // Check for user-specified port number
    if(argC == 2){

        *portNum = atoi(argV[1]);
        if((*portNum < 60000) || (*portNum > 60099)){

            printf("\n*Specify a port number between 60000 and 60099.\n\n");
            exit(EXIT_FAILURE);

        }

    }
    // Use default port 60032
    else
        *portNum = DEF_PORT;

    return;

}

void buildResponse(char *buff, char **packet){

    int filesize = 0;
    char method[8] = {0};
    char filename[DIRECTORY_SIZE] = {0};
    char version[9] = {0};
    char type[50] = {0};
    char header[100] = {0};
    char *data = NULL;
    FILE *infile = NULL;

    // Parse request line from Client
    sscanf(buff, "%s %s %s", method, filename, version);
    if(strcmp(filename, "/") == 0)
        strcpy(filename, "/index.html");
    strcpy(filename, filename + 1);

    // Check for and set common file types
    if(strstr(filename, ".htm") != NULL)
        strcpy(type, "text/html");
    else if(strstr(filename, ".ico") != NULL)
        strcpy(type, "image/vnd.microsoft.icon");
    else if(strstr(filename, ".gif") != NULL)
        strcpy(type, "image/gif");
    else if(strstr(filename, ".png") != NULL)
        strcpy(type, "image/png");
    else if((strstr(filename, ".jpg") != NULL) || (strstr(filename, ".jpeg")
        != NULL))
        strcpy(type, "image/jpeg");
    else
        strcpy(type, "unknown");
    
    // Access requested file
    infile = fopen(filename, "rb");
    if(infile != NULL){

        fseek(infile, 0, SEEK_END);
        filesize = ftell(infile);
        fseek(infile, 0, SEEK_SET);
        data = malloc((sizeof(char) * filesize) + 1);
        //memset(data, 0, filesize + 1);
        fread(data, filesize, 1, infile);

        // Assemble response header
        sprintf(header, "%s 200 OK\nContent-Type: %s\nContent-Length: %d\n\n",
            version, type, filesize);

        // Assemble full response
        *packet = malloc(strlen(header) + filesize + 1);
        strcpy(*packet, header);
        strcat(*packet, data);

        free(data);
        fclose(infile);

    }
    else{

        sprintf(header, "%s 404 Not Found\nContent-Length: 0\nContent-Type: "
            "%s\n\n", version, type);

        *packet = malloc(strlen(header) + 1);
        strcpy(*packet, header);

    }

    return;

}

void serverIntercom(int serverFD, struct sockaddr_in *servAddr){

    int clientFD = 0;
    int addrLen = 0;
    int long valRead = 0;
    char buff[BUFFER_SIZE] = {0};
    char *packet = NULL;

    addrLen = sizeof(struct sockaddr_in);
    
    printf("++++++++++ Waiting for new connection ++++++++++\n\n");

    clientFD = accept(serverFD, (struct sockaddr *)servAddr,
        (socklen_t *)&addrLen);
    if(clientFD < 0){

        fprintf(stderr, "Connection terminated unexpectedly.\n\n");
        close(serverFD);
        exit(EXIT_FAILURE);

    }

    // Receive message from Client
    valRead = recv(clientFD, buff, BUFFER_SIZE, 0);
    if(valRead > 0){

        // Echo HTTP request
        printf("%s\n\n", buff);

        // Construct HTTP response
        buildResponse(buff, &packet);

        // Send HTTP response to Client
        send(clientFD, packet, strlen(packet), 0);

        printf("--------------- Response sent -----------------\n\n"
            "%s\n\n", packet);

        free(packet);

    }
    else if(valRead == 0)
        fprintf(stderr, "Client message was empty.\n\n");
    else
        fprintf(stderr, "Error reading client message.\n\n");

    close(clientFD);

    return;

}

void sigintHandler(int sig){

    // Verify server process is ending
    printf("\tShutting down server...\n\n");

    // Return SIGINT
    exit(sig);

}

