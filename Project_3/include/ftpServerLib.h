#ifndef FTP_SERVER_LIB_H
#define FTP_SERVER_LIB_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <pthread.h>
    #include <signal.h>

    // Temp
    #include <sys/stat.h>
    #include <sys/sendfile.h>
    #include <fcntl.h>
    #include <arpa/inet.h>
    #include <stdbool.h>

    // For LS
    #include <dirent.h>

    // For forking
    #include <sys/types.h>

    #define DEF_PORT 8080
    #define COMM_SIZE 15

    typedef struct conn_t{

        int socket;
        struct sockaddr address;
        int addrLen;

    }conn_t;

    int setPort(int argC, char **argV);
    void initServer(int *servSock, struct sockaddr_in *servAddr, int portNum);
    void connectClient(int servSock, conn_t *connection, pthread_t *thread);
    void *threadedMonitor(void *ptr);
    void *threadedHandler(void *ptr);
    void sigintHandler(int sig);

    // Temp
    void parseFilename(char *msg, char *filename);
    bool transmitFile(int socket, char *filename);

    // For Forking
    int serverMenu(void);

#endif /* FTP_SERVER_LIB_H */

