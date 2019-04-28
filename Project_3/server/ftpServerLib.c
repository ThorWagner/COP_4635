#include "../include/ftpServerLib.h"
#include "../include/global.h"

/* -- Global Variables -- */

// Flag to stop threaded handling of clients
extern bool g_running;

// Counter to keep track of active users
extern int g_current;

// Counter to keep track of total visitors
extern int g_all;

// Counter to keep track of active users
//volatile int g_current;

// Counter to keep track of total system visitors
//volatile int g_all;

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

void *threadedHandler(void *ptr){

//    int length = 0;
//    long int address = 0;
//    char *buffer = NULL;
    bool running = false;
    conn_t *connection = NULL;

    // Temp
    char request[BUFSIZ] = {0};
    char response[BUFSIZ] = {0};
    char filename[BUFSIZ] = {0};

    // For LS
    struct dirent *de = NULL;
    DIR *dr = NULL;

    // For GET
    int newFD = 0;
    int filesize = 0;
    char *data = NULL;

    if(!ptr)
        pthread_exit(0);

    connection = (conn_t *)ptr;
/*    read(connection->socket, &length, sizeof(int));
    if(length > 0){

        address = (long int)((struct sockaddr_in *)&connection->address)->sin_addr.s_addr;
        buffer = (char *)malloc((length + 1) * sizeof(char));
        memset(buffer, 0, length + 1);

        read(connection->socket, buffer, length);

        printf("%d.%d.%d.%d: %s\n",
            (int)(address & 0xff),
            (int)((address >> 8) & 0xff),
            (int)((address >> 16) & 0xff),
            (int)((address >> 24) & 0xff),
            buffer);
        free(buffer);

    }
    else if(length == 0)
        fprintf(stderr, "\nClient message was empty.\n\n");
    else
        fprintf(stderr, "\nError reading client message.\n\n");
*/

    // Temp
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
/*
void sigintHandler(int sig){

    // Verify server process is ending
    fprintf(stderr, "\n\tShutting down server...\n\n");

    // Return SIGINT
    exit(sig);

}
*/
// Temp
void parseFilename(char *msg, char *filename){

    char *temp = NULL;

    temp = strchr(msg, ' ');
    strcpy(filename, temp + 1);

    return;

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

