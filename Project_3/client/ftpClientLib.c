/** @file ftpClientLib.c
 *  @brief Overall process management for the FTP Client.
 *
 *  This file contains function definitions to initialize and run the FTP
 *  Client with persistant communication to the FTP Server and ability to
 *  respond to continuous user input.
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
 * <stdlib.h>           - Needed for malloc(), free(), sizeof(), and NULL
 * <string.h>           - Needed for strcpy(), strcat(), strcmp(), strcasecmp(),
 *                        strncasecmp(), andmemset()
 * <unistd.h>           - Needed for close() and getcwd()
 * <sys/socket.h>       - Needed for socket() and connect()
 * <arpa/inet.h>        - Needed for struct sockaddr_in
 * <sys/stat.h>         - Needed for struct stat and stat()
 * <sys/sendfile.h>     - Needed for sendfile()
 * <fcntl.h>            - Needed for open()
 *
 */

/** @brief Sets the value of the port based on user input.
 *  
 *  This function will set the port according to user input and check the
 *  validity of the provided value. If the program is initiated without a
 *  user-specified port number the port will be set to the default value
 *  defined by DEF_PORT.
 *
 *  @param argC - Provided by the system, indicates the number of arguments
 *  @param argV - Provided by the system, contains all argument strings
 *  @return portNum - Initialized port number based on user input
 */
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

/** @brief Initializes the Client connection to the Server.
 *  
 *  This function attempts to start the Client on the specified port and
 *  complete the connection to the Server. Will provide messages
 *  indicating errors or when the Client has been successfully started.
 *
 *  @param newSock - Pointer to the socket descriptor to initialize
 *  @param address - Pointer to the address structure to initialize
 *  @param portNum - Port number to connect to
 *  @return void
 */
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

/** @brief Displays the Client menu and gets user selection.
 *  
 *  This function displays all available Client commands and waits for a
 *  user selection. Selections are tested for validity and matched to a
 *  numerical representation.
 *
 *  @param void
 *  @return opt - Integer value representing the command selected by user
 */
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
        else
            printf("\n\'%s\' is not a valid command.\n\n", command);

    }while(opt == 0);

    return opt;

}

/** @brief Sends the designated file over the designated port.
 *  
 *  This function transmits a file from one end of the connection to the
 *  other. Identifies statistics about the target file (i.e. size) to allow
 *  the recipient to prepare for the transfer.
 *
 *  @param socket - Socket descriptor indicating where to transmit file
 *  @param filename - String containg the name of the file to transmit
 *  @return - Returns true to indicate successfull transmittal
 */
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

