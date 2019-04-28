#ifndef FTP_CLIENT_LIB_H
#define FTP_CLIENT_LIB_H

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

    #define DEF_PORT 8080
    #define COMM_SIZE 15

    int setPort(int argC, char **argV);
    void initClient(int *newSock, struct sockaddr_in *address, int portNum);
    int clientMenu(void);
    bool transmitFile(int socket, char *filename);

#endif /* FTP_CLIENT_LIB_H */

