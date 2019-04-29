/** @file ftpClient.c
 *  @brief Main Client program driver
 *
 *  This file contains the main program body for the FTP Client. It relies on
 *  function definition and static definitions made in 'ftpClientLib.c' and
 *  'ftpClientLib.h'. A persistant TCP connection is formed with the Server so
 *  the client can make multiple requests.
 *
 *  @author Michael Wagner
 *  @date 04/28/2019
 *  @info Course COP 4635
 *  @bug No known bugs.
 */

/* -- Includes -- */

/* FTP Client header */
#include "../include/ftpClientLib.h"

/* Inherited from header files:
 *
 * <stdio.h>            - Needed for printf(), fprintf(), and BUFSIZ
 * <stdlib.h>           - Needed for NULL
 * <string.h>           - Needed for memset()
 * <stdbool.h>          - Needed for boolean data type
 * <unistd.h>           - Needed for close()
 * <fcntl.h>            - Needed for open()
 * <arpa/inet.h>        - Needed for struct sockaddr_in
 *
 */

/** @brief Main Client program driver.
 *  
 *  Primary functionality of the FTP Client program. Attempts to start the
 *  Client on the specified port (or default if none provided) and connected to
 *  FTP Sever, continually asks for user commands until exited by user. Allows
 *  the user to 1) Display files on the FTP Server, 2) Download file from FTP
 *  Server, 3) Upload file to the FTP Server, 4) Display the working directory,
 *  5) Ask for sarcastic help, and 6) Exit the Client.
 *
 *  @param argC - Provided by the system, indicates the number of arguments
 *  @param argV - Provided by the system, contains all argument strings
 *  @return 0 on exit
 */
int main(int argC, char **argV){

    int portNum = 0;
    int newSock = 0;
    int opt = 0;
    int newFD = 0;
    int filesize = 0;
    char msg1[BUFSIZ] = {0};
    char msg2[BUFSIZ] = {0};
    char filename[BUFSIZ] = {0};
    char *data = NULL;
    bool running = false;
    struct sockaddr_in address;

    if(argC > 2){

        fprintf(stderr, "\nProper usage: ./client   OR   ./client [port]\n\n");
        return 1;

    }

    portNum = setPort(argC, argV);

    initClient(&newSock, &address, portNum);

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
                strcpy(msg1, "LOGOFF");
                write(newSock, msg1, strlen(msg1));
                running = false;
                break;

            case 2:
                strcpy(msg1, "LS");
                write(newSock, msg1, strlen(msg1));
                recv(newSock, msg2, BUFSIZ, 0);
                printf("\n%s\n\n", msg2);
                break;

            case 3:
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

