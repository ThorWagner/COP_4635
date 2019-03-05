#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>

    #define DEF_PORT 60032
    #define BUFFER_SIZE 1024
    #define WAIT_SIZE 10

    void showError(char *errorMSG);

#endif /* CHAT_SHARED_H */

