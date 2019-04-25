#ifndef FTP_SERVER_H
#define FTP_SERVER_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/stat.h>
    #include <sys/sendfile.h>
    #include <pthread.h>
    #include <signal.h>

    #define DEF_PORT 8080
    #define WAIT_SIZE 10
    #define PARAM_SIZE 1024
    #define BUFFER_SIZE 4096

#endif /* FTP_SERVER_H */

