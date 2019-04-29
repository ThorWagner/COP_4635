/** @file ftpServerLib.h
 *  @brief Overall process management for the FTP Client
 *
 *  This file contains the prototyping and static definitions used to initialize
 *  and run the FTP Server with persistant communication to FTP Clients via
 *  independent threads and respond to continuous user input.
 *
 *  @author Michael Wagner
 *  @date 04/28/2019
 *  @info Course COP 4635
 *  @bug No known bugs.
 */

/* -- Includes -- */

/* FTP Server header */
#include "../include/ftpServerLib.h"

/* Global Variable heder */
#include "../include/global.h"

/* Inherited from header files:
 *
 * <stdio.h>            - Needed for printf(), fprintf(), and BUFSIZ
 * <stdlib.h>           - Needed for malloc(), free(), and NULL
 * <string.h>           - Needed for strcpy(), strcat(), strcmp(), strcasecmp(),
 *                      - strncasecmp(), and memset()
 * <stdbool.h>          - Needed for boolean data type
 * <unistd.h>           - Needed for close()
 * <fcntl.h>            - Needed for open()
 * <sys/socket.h>       - Needed for socket(), bind(), listen(), and accept()
 * <pthread.h>          - Needed for pthread_create(), pthread_detach(),
 *                      - pthread_exit(), pthread_join(), and pthread_t data type
 * <sys/stat.h>         - Needed for sturct stat and stat()
 * <sys/sendfile.h>     - Needed for sendfile()
 * <arpa/inet.h>        - Needed for struct sockaddr_in
 * <dirent.h>           - Needed for opendir(), readdir(), closedir(), struct
 *                      - dirent, and DIR data type
 *
 */

/* -- Global Variables -- */

// Flag to stop threaded handling of clients
extern bool g_running;

// Counter to keep track of active users
extern int g_current;

// Counter to keep track of total visitors
extern int g_all;

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

/** @brief Initializes the Server and begins waiting for Client connections
     *  
     *  This function attempts to start the Server on the specified port and
     *  begin listening for incoming connections on it. Will provide messages
     *  indicating errors or when the Server has been successfully started.
     *
     *  @param newSock - Pointer to the socket descriptor to initialize
     *  @param address - Pointer to the address structure to initialize
     *  @param portNum - Port number to connect to
     *  @return void
     */
void initServer(int *servSock, struct sockaddr_in *servAddr, int portNum){

    int addrLen = 0;
    int status = 0;

    addrLen = sizeof(struct sockaddr_in);

    *servSock = socket(AF_INET, SOCK_STREAM, 0);
    if(*servSock <= 0){

        fprintf(stderr, "\nFailed to create socket.\n\n");
        exit(EXIT_FAILURE);

    }

    memset(servAddr, 0, addrLen);
    servAddr->sin_family = AF_INET;
    servAddr->sin_addr.s_addr = INADDR_ANY;
    servAddr->sin_port = htons(portNum);

    status = bind(*servSock, (struct sockaddr *)servAddr, addrLen);
    if(status < 0){

        fprintf(stderr, "\nFailed to bind socket to port %d.\n\n", portNum);
        close(*servSock);
        exit(EXIT_FAILURE);

    }

    status = listen(*servSock, 10);
    if(status < 0){

        fprintf(stderr, "\nFailed to listen on port %d.\n\n", portNum);
        close(*servSock);
        exit(EXIT_FAILURE);

    }

    printf("\nServer started on port: %d\n", portNum);

    return;

}

/** @brief Accepts incoming connections and starts a thread for each
     *  
     *  This function detect the incoming connections from Clients and accepts
     *  them. Once accepted a new thread is created to handle all requests from
     *  the Client until it disconnects.
     *
     *  @param servSock - Socket descriptor of the Server
     *  @param connection - Pointer to data structure describing connection
     *  @param thread - Pointer to modify and place TID in after creation
     *  @return void
     */
void connectClient(int servSock, conn_t *connection, pthread_t *thread){


    connection = (conn_t *)malloc(sizeof(conn_t));
    connection->socket = accept(servSock, &connection->address,
        (socklen_t *)&connection->addrLen);
    if(connection->socket <= 0){

        if(g_running == true)
            printf("Error connecting to client.\n\n");
        free(connection);

    }
    else{

        pthread_create(thread, 0, threadedHandler, (void *)connection);
        pthread_detach(*thread);

    }   

    return;

}

/** @brief Thread that monitors incoming Client connections
     *  
     *  Designed to be passed to a pthread_create() call so one thread can
     *  monitor incoming FTP Client connections and the other thread can
     *  maintain a user interface for Server commands.
     *
     *  @param ptr - Pointer to the Server socket descriptor
     *  @return void
     */
void *threadedMonitor(void *ptr){

    int *servSock = NULL;
    conn_t *connection = NULL;
    pthread_t thread;

    servSock = (int *)ptr;

    while(g_running == true)
        connectClient(*servSock, connection, &thread);

    printf("\nTerminating threads...\n\n");

    pthread_exit(0);

}

/** @brief Thread that handles persistent connections with Client
     *  
     *  Designed to be passed to a pthread_create() call so the connection
     *  monitoring thread can create a new thread to handle each new Client
     *  connection.
     *
     *  @param ptr - Pointer to data structure defining the new connection
     *  @return void
     */
void *threadedHandler(void *ptr){

    int newFD = 0;
    int filesize = 0;
    char request[BUFSIZ] = {0};
    char response[BUFSIZ] = {0};
    char filename[BUFSIZ] = {0};
    char *data = NULL;
    bool running = false;
    conn_t *connection = NULL;
    struct dirent *de = NULL;
    DIR *dr = NULL;

    if(!ptr)
        pthread_exit(0);

    connection = (conn_t *)ptr;

    running = true;
    do{

        memset(request, 0, BUFSIZ);
        memset(response, 0, BUFSIZ);
        memset(filename, 0, BUFSIZ);
        recv(connection->socket, request, BUFSIZ, 0);
        if(strncasecmp(request, "LOGON", (size_t)5) == 0){

            g_current++;
            g_all++;

        }
        else if(strncasecmp(request, "LS", (size_t)2) == 0){

            dr = opendir(".");
            if(dr == NULL)
                strcpy(response, "Unable to access current directory.");
            else{

                while((de = readdir(dr)) != NULL){

                    strcat(response, de->d_name);
                    strcat(response, "\n");

                }

                closedir(dr);

            }

            write(connection->socket, response, strlen(response));

        }
        else if(strncasecmp(request, "GET", (size_t)3) == 0){

            parseFilename(request, filename);
            if(access(filename, F_OK) != -1){

                strcpy(response, "OK");
                write(connection->socket, response, strlen(response));
                transmitFile(connection->socket, filename);

            }
            else{

                strcpy(response, "NO");
                write(connection->socket, response, strlen(response));

            }

        }
        else if(strncasecmp(request, "PUT", (size_t)3) == 0){

            parseFilename(request, filename);
            strcpy(response, "OK");
            write(connection->socket, response, strlen(response));
            recv(connection->socket, &filesize, sizeof(int), 0);
            data = malloc(filesize);
            newFD = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
            recv(connection->socket, data, filesize, 0);
            write(newFD, data, filesize);
            free(data);
            close(newFD);

        }
        else if(strncasecmp(request, "LOGOFF", (size_t)6) == 0){

            running = false;
            g_current--;

        }

    }while(running);

    close(connection->socket);
    free(connection);
    pthread_exit(0);

}

/** @brief Parses the filenames from GET and PUT requests
     *  
     *  This function parses the name of a file from the full message of a GET
     *  or PUT request received from an FTP Client.
     *
     *  @param msg - String containing the full request
     *  @param filename - String to place the parsed filename into
     *  @return void
     */
void parseFilename(char *msg, char *filename){

    char *temp = NULL;

    temp = strchr(msg, ' ');
    strcpy(filename, temp + 1);

    return;

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

/** @brief Displays the Client menu and gets user selection.
     *  
     *  This function displays all available Client commands and waits for a
     *  user selection. Selections are tested for validity and matched to a
     *  numerical representation.
     *
     *  @param void
     *  @return opt - Integer value representing the command selected by user
     */
int serverMenu(void){

    int opt = 0;
    char command[COMM_SIZE] = {0};

    do{

        printf("\n-=| SERVER COMMAND |=-\n"
            "help: Display all Server Commands\n"
            "quit: Close the FTP Server\n"
            "count current: Display the number of active users\n"
            "count all: Display the total number of system visitors\n\n"
            "Enter an action: ");
        scanf("\n\n%[^\n]", command);

        if(strcasecmp(command, "help") == 0){

            printf("\nAll the commands are self-explanatory...\n");

        }
        else if(strcasecmp(command, "quit") == 0)
            opt = 1;
        else if(strcasecmp(command, "count current") == 0)
            opt = 2;
        else if(strcasecmp(command, "count all") == 0)
            opt = 3;

    }while(opt == 0);

    return opt;

}

