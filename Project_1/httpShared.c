#include <arpa/inet.h>
#include "httpShared.h"

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

void setPort(int argC, char **argV, int *portNum, int numArgs){

    // Check for user-specified port number
    if(argC == numArgs){

        *portNum = atoi(argV[numArgs - 1]);
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

void buildHeader(char *buff, char *header, char *filename, int *filesize){

    char method[ARG_SIZE] = {0};
    char version[ARG_SIZE] = {0};
    char type[ARG_SIZE] = {0};
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
    else if(strstr(filename, ".jp") != NULL)
        strcpy(type, "image/jpeg");
    else
        strcpy(type, "unknown");

    // Access requested file
    infile = fopen(filename, "rb");
    if(infile != NULL){

        // Calculate filesize
        fseek(infile, 0, SEEK_END);
        *filesize = ftell(infile);
        fseek(infile, 0, SEEK_SET);

        // Assemble 'OK' response header
        sprintf(header, "%s 200 OK\nContent-Length: %d\nContent-Type: %s\n\n",
            version, *filesize, type);

        // Close file pointer
        fclose(infile);

    }
    else{

        // Calculate new filesize
        strcpy(filename, "error404.html");
        infile = fopen(filename, "rb");
        fseek(infile, 0, SEEK_END);
        *filesize = ftell(infile);
        fseek(infile, 0, SEEK_SET);
        
        // Assemble 'Not Found' response header
        sprintf(header, "%s 404 Not Found\nContent-Length: %d\nContent-Type: "
            "%s\n\n", version, *filesize, "text/html");

    }

    return;

}

void serverIntercom(int serverFD, struct sockaddr_in *servAddr){

    int clientFD = 0;
    int addrLen = 0;
    int filesize = 0;
    int long valRead = 0;
    char buff[BUFFER_SIZE] = {0};
    char header[HEADER_SIZE] = {0};
    char filename[ARG_SIZE] = {0};
    char data[MAX_FILESIZE] = {0};
    FILE *infile = NULL;

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
        printf("%s", buff);

        // Construct HTTP response
        buildHeader(buff, header, filename, &filesize);

        // Send HTTP response to Client
        send(clientFD, header, strlen(header), 0);
        infile = fopen(filename, "rb");
        fread(data, 1, filesize + 1, infile);
        send(clientFD, data, filesize, 0);
        fclose(infile);

        printf("--------------- Response sent -----------------\n\n%s", header);

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
    char data[MAX_FILESIZE] = {0};
    char header[HEADER_SIZE] = {0};

    addrLen = sizeof(struct sockaddr_in);

    // Construct request header based on user input
    buildRequest(file, header, portNum);

    // Connect to Server
    printf("\n++++++++ Attempting to connect ++++++++\n\n");
    status = connect(clientFD, (struct sockaddr *)servAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "Failed to connect to server.\n\n");
        close(clientFD);
        exit(EXIT_FAILURE);

    }
    
    // Send request to Server
    send(clientFD, header, strlen(header), 0);
    printf("------------ Request sent ------------\n\n%s", header);

    // Receive response from Server
    valRead = recv(clientFD, data, MAX_FILESIZE, 0);
    if(valRead > 0)
        printf("---------- Response received ----------\n\n%s"
            "----------- End of response -----------\n\n", data);
    else if(valRead == 0)
        fprintf(stderr, "Server message was empty.\n\n");
    else
        fprintf(stderr, "Error reading server message.\n\n");

    return;

}

