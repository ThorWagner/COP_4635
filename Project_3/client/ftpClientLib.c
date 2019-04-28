#include "ftpClientLib.h"

int setPort(int argC, char **argV){

    int portNum = 0;

    if(argC == 2){

        portNum = atoi(argV[1]);
        if(portNum <= 0){

            fprintf(stderr, "\nInvalid port number.\n\n");
            exit(EXIT_FAILURE);

        }

    }
    else
        portNum = DEF_PORT;

    return portNum;

}

void initClient(int *newSock, struct sockaddr_in *address, int portNum){

    int addrLen = 0;
    int status = 0;

    addrLen = sizeof(struct sockaddr_in);

    *newSock = socket(AF_INET, SOCK_STREAM, 0);
    if(*newSock <= 0){

        fprintf(stderr, "\nFailed to create socket.\n\n");
        exit(EXIT_FAILURE);

    }

    memset(address, 0, addrLen);
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(portNum);

    status = connect(*newSock, (struct sockaddr *)address, addrLen);
    if(status != 0){

        fprintf(stderr, "\nFailed to connect to host.\n\n");
        close(*newSock);
        exit(EXIT_FAILURE);

    }

    printf("\nClient started on port: %d\n\n", portNum);

    return;

}

int clientMenu(void){

    int opt = 0;
    char command[COMM_SIZE] = {0};

    do{

        printf("-=| CLIENT COMMAND |=-\n"
            "help: Display all Client Commands\n"
            "quit: Close the FTP Client\n"
            "ls: Display all files on the FTP Server\n"
            "get: Download a file from the FTP Server\n"
            "put: Upload a file to the FTP Server\n"
            "pwd: Display the current file path\n\n"
            "Enter an action: ");
        scanf("\n\n%s", command);

        if(strcasecmp(command, "help") == 0)
            printf("\nAll the commands are self-explanatory...\n\n");
        else if(strcasecmp(command, "quit") == 0)
            opt = 1;
        else if(strcasecmp(command, "ls") == 0)
            opt = 2;
        else if(strcasecmp(command, "get") == 0)
            opt = 3;
        else if(strcasecmp(command, "put") == 0)
            opt = 4;
        else if(strcasecmp(command, "pwd") == 0)
            opt = 5;

    }while(opt == 0);

    return opt;

}

bool transmitFile(int socket, char *filename){

    int newFD = 0;
    int filesize = 0;
    struct stat obj;

    stat(filename, &obj);
    newFD = open(filename, O_RDONLY);
    filesize = obj.st_size;
    send(socket, &filesize, sizeof(int), 0);
    sendfile(socket, newFD, NULL, filesize);
    close(newFD);

    return true;

}

