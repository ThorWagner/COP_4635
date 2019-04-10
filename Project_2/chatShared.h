/** @file chatClient.h
 *  @brief Header file for shared functionality between Client and Server.
 *
 *  This file contains all of the shared libraries, definitions, and functions
 *  for chatServer.c and chatClient.c.
 *
 *  @author Michael Wagner
 *  @date 04/09/2019
 *  @info Course COP 4635 - Project 2
 *  @bug No known bugs.
 */

#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <errno.h>

    #define DEF_PORT 60032
    #define BUFFER_SIZE 1024
    #define WAIT_SIZE 10
    #define PARAM_SIZE 20

    void showError(char *errorMSG);

#endif /* CHAT_SHARED_H */

