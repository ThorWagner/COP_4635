#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

// Temp
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#include "ftpClientLib.h"

//#define NAME_SIZE 50

int setPort(int argC, char **argV);
void initClient(int *newSock, struct sockaddr_in *address, int portNum);
int clientMenu(void);
bool transmitFile(int socket, char *filename);

int main(int argC, char **argV){

    int portNum = 0;
    int newSock = 0;
    int opt = 0;
    char filename[BUFSIZ] = {0};
    bool running = false;
    //int length = 0;
    //int status = 0;
    struct sockaddr_in address;
    //struct hostent *host = NULL;

    // Temp
    int newFD = 0;
    int filesize = 0;
    char msg1[BUFSIZ] = {0};
    char msg2[BUFSIZ] = {0};
    char *data = NULL;

/*    if(argC != 4){

        printf("Proper usage: ./client [hostname] [port] [message]\n");
        return 1;

    }
*/

    if(argC > 2){

        fprintf(stderr, "\nProper usage: ./client   OR   ./client [port]\n\n");
        return 1;

    }

/*    portNum = atoi(argV[2]);
    if(portNum <= 0){

        fprintf(stderr, "Invalid port number.\n");
        return 1;

    }
*/

    portNum = setPort(argC, argV);

/*    newSock = socket(AF_INET, SOCK_STREAM, 0);
    if(newSock <= 0){

        fprintf(stderr, "Could not create socket.\n");
        return 1;

    }

    address.sin_family = AF_INET;
    address.sin_port = htons(portNum);
    host = gethostbyname(argV[1]);
    if(!host){

        fprintf(stderr, "Unknown host '%s'.\n", argV[1]);
        return 1;

    }

    memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
    status = connect(newSock, (struct sockaddr *)&address, sizeof(address));
    if(status != 0){

        fprintf(stderr, "Could not connect to host '%s'.\n", argV[1]);
        return 1;

    }
*/

    initClient(&newSock, &address, portNum);

/*    length = strlen(argV[3]);
    write(newSock, &length, sizeof(int));
    write(newSock, argV[3], length);
    close(newSock);
*/

    // Temp
/*    strcpy(msg1, "GET ");
    strcat(msg1, "a.txt");
    write(newSock, msg1, strlen(msg1));
    recv(newSock, msg2, 2, 0);
    if(strcmp(msg2, "OK") == 0){

        recv(newSock, &filesize, sizeof(int), 0);
        data = malloc(filesize);
        newFD = open("a.txt", O_CREAT | O_EXCL | O_WRONLY, 0666);
        recv(newSock, data, filesize, 0);
        write(newFD, data, filesize);
        close(newFD);

    }
    else
        fprintf(stderr, "\nInvalid filename.\n\n");
*/

    strcpy(msg1, "LOGON");
    write(newSock, msg1, strlen(msg1));
    running = true;
    do{
    
        memset(msg1, 0, BUFSIZ);
        memset(msg2, 0, BUFSIZ);
        memset(filename, 0, BUFSIZ);
        
        opt = clientMenu();
        switch(opt){

            case 1:
                //memset(msg1, 0, BUFSIZ);
                strcpy(msg1, "LOGOFF");
                write(newSock, msg1, strlen(msg1));
                running = false;
                break;

            case 2:
                //memset(msg1, 0, BUFSIZ);
                strcpy(msg1, "LS");
                write(newSock, msg1, strlen(msg1));
                recv(newSock, msg2, BUFSIZ, 0);
                printf("\n%s\n\n", msg2);
                break;

            case 3:
                //memset(filename, 0, NAME_SIZE);
                //memset(msg1, 0, BUFSIZ);
                //memset(msg2, 0, BUFSIZ);
                printf("\nEnter the filename: ");
                scanf("\n\n%s", filename);
                strcpy(msg1, "GET ");
                strcat(msg1, filename);
                write(newSock, msg1, strlen(msg1));
                recv(newSock, msg2, 2, 0);
                if(strcmp(msg2, "OK") == 0){

                    printf("\nDownloading \'%s\'\n\n", filename);
                    recv(newSock, &filesize, sizeof(int), 0);
                    data = malloc(filesize);
                    newFD = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                    recv(newSock, data, filesize, 0);
                    write(newFD, data, filesize);
                    free(data);
                    close(newFD);

                }
                else
                    printf("\nInvalid filename.\n\n");
                break;

            case 4:
                //memset(filename, 0, NAME_SIZE);
                //memset(msg1, 0, BUFSIZ);
                //memset(msg2, 0, BUFSIZ);
                printf("\nEnter the filename: ");
                scanf("\n\n%s", filename);
                printf("\n");
                strcpy(msg1, "PUT ");
                strcat(msg1, filename);
                write(newSock, msg1, strlen(msg1));
                recv(newSock, msg2, 2, 0);
                if(strcmp(msg2, "OK") == 0){

                    printf("\nUploading \'%s\'\n\n", filename);
                    transmitFile(newSock, filename);

                }
                break;

            case 5:
                //memset(msg1, 0, BUFSIZ);
                if(getcwd(msg1, BUFSIZ) != NULL)
                    printf("\nCurrent working directory:\n\n\t%s\n\n", msg1);
                else
                    printf("\nUnable to retrieve file path.\n\n");
                break;

            default:
                printf("\nAn error occured.\n\n");
                break;

        }

    }while(running);

    printf("\nShutting down client...\n\n");
    close(newSock);

    return 0;

}

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

