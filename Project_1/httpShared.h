/** @file httpShared.h
 *  @brief All necessary resources for httpServer.c and httpClient.c
 *
 *  This file contains all of the shared libraries and definition for
 *  httpServer.c and httpClient.c. It also contains all prototypes of functions
 *  used by those aforementioned files.
 *
 *  @author Michael Wagner
 *  @date 02/24/2019
 *  @info Course COP 4635
 *  @bug No known bugs
 */

#ifndef HTTP_SHARED_H
#define HTTP_SHARED_H

    /* -- Includes -- */
    
    /** Standard I/O library
     *  
     *  Resources used - printf(), fprintf(), and stderr
     */
    #include <stdio.h>
    
    /** Standard library
     *  
     *  Resources used - sizeof(), exit(), NULL, and EXIT_FAILURE
     */
    #include <stdlib.h>
    
    /** String library
     *  
     *  Resources used - strlen(), strcpy(), strstr(), and memset()
     */
    #include <string.h>

    /** UNIX Standard library
     *  
     *  Resources used - close()
     */
    #include <unistd.h>

    /** Internet Address library
     *  
     *  Resources used - htons(), struct sockadrr_in, struct sockaddr, and
     *  INADDR_ANY
     */
    #include <netinet/in.h>

    /** Main Sockets library
     *  
     *  Resources used - socket(), bind(), listen(), accept(), connect(),
     *  send(), recv(), AF_INET, and SOCK_STREAM
     */
    #include <sys/socket.h>
    
    /* -- Definitions -- */
    
    // Global definition for the size of a buffer
    #define BUFFER_SIZE 1024
    
    // Global definition for the size of a header
    #define HEADER_SIZE 512
    
    // Global definition for the size of an arbitrary argument
    #define ARG_SIZE 100
    
    // Global definition for the default connection port
    #define DEF_PORT 60032
    
    // Global definition for the maximum number of queued connections
    #define WAIT_SIZE 10
    
    // Global definition for the maximum filesize that the server will read
    #define MAX_FILESIZE 100000

    /* -- Function Prototypes -- */
    
    /** @brief Initializes the server file descriptor and address
     *  
     *  Given pointers to the server file descriptor and server address, this
     *  function will generate both based on the port number (given or default).
     *
     *  @param serverFD - pointer to the server file descriptor
     *  @param servAddr - pointer to the server address
     *  @param portNum - the port number to initialize on
     *  @return void
     */
    void initServer(int *serverFD, struct sockaddr_in *servAddr, int portNum);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void setPort(int argC, char **argV, int *portNum, int numArgs);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void buildHeader(char *buff, char *header, char *filename, int *filesize);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void serverIntercom(int serverFD, struct sockaddr_in *servAddr);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void sigintHandler(int sig);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void initClient(int *clientFD, struct sockaddr_in *servAddr, int portNum);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void buildRequest(char *file, char *header, int portNum);
    
    /** @brief
     *  
     *  blah
     *
     *  @param
     *  @return void
     */
    void clientIntercom(int clientFD, struct sockaddr_in *servAddr, int portNum, char *file);

#endif /* HTTP_SHARED_H */

