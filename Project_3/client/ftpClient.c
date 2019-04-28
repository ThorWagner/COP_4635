#include "ftpClientLib.h"

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

