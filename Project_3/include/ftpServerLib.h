/** @file ftpServerLib.h
 *  @brief Server specific prerequisites and functionalities
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

#ifndef FTP_SERVER_LIB_H
#define FTP_SERVER_LIB_H

    /* -- Includes -- */

    /* Standard I/O Library */
    #include <stdio.h>          // Needed for printf(), fprintf(), and BUFSIZ

    /* Standard Library */
    #include <stdlib.h>         // Needed for malloc(), free(), and NULL

    /* String Library */
    #include <string.h>         // Needed for strcpy(), strcat(), strcmp(),
                                // strcasecmp(), strncasecmp(), and memset()

    /* Standard Boolean Library */
    #include <stdbool.h>        // Needed for boolean data type

    /* UNIX Standard Library */
    #include <unistd.h>         // Needed for close() and getcwd()

    /* File Control Library */
    #include <fcntl.h>          // Needed for open()

    /* Socket Library */
    #include <sys/socket.h>     // Needed for socket() and connect()

    /* Pthread Library */
    #include <pthread.h>        // Needed for pthread_create(),
                                // pthread_detach(), pthread_exit(), and
                                // pthread_t data type

    /* File Stat Library */
    #include <sys/stat.h>       // Needed for struct stat and stat()

    /* File Transfer Library */
    #include <sys/sendfile.h>   // Needed for sendfile()

    /* Internet Protocols Library */
    #include <arpa/inet.h>      // Needed for struct sockaddr_in

    /* Directory Entries Library */
    #include <dirent.h>         // Needed for opendir(), readdir(), closedir(),
                                // sturct dirent, and DIR data type

    /* -- Global Definitions -- */

    // Defines the default port for FTP communication
    #define DEF_PORT 8080

    // Defines the maximum size for command string
    #define COMM_SIZE 15

    /* -- Data Structures -- */

    /* Connection data structure */
    typedef struct conn_t{

        int socket;
        struct sockaddr address;
        int addrLen;

    }conn_t;

    /** @brief Sets the value of the port based on user input
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
    int setPort(int argC, char **argV);
    
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
    void initServer(int *servSock, struct sockaddr_in *servAddr, int portNum);
    
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
    void connectClient(int servSock, conn_t *connection, pthread_t *thread);
    
    /** @brief Thread that monitors incoming Client connections
     *  
     *  Designed to be passed to a pthread_create() call so one thread can
     *  monitor incoming FTP Client connections and the other thread can
     *  maintain a user interface for Server commands.
     *
     *  @param ptr - Pointer to the Server socket descriptor
     *  @return void
     */
    void *threadedMonitor(void *ptr);
    
    /** @brief Thread that handles persistent connections with Client
     *  
     *  Designed to be passed to a pthread_create() call so the connection
     *  monitoring thread can create a new thread to handle each new Client
     *  connection.
     *
     *  @param ptr - Pointer to data structure defining the new connection
     *  @return void
     */
    void *threadedHandler(void *ptr);
    
    
//    void sigintHandler(int sig);

    /** @brief Parses the filenames from GET and PUT requests
     *  
     *  This function parses the name of a file from the full message of a GET
     *  or PUT request received from an FTP Client.
     *
     *  @param msg - String containing the full request
     *  @param filename - String to place the parsed filename into
     *  @return void
     */
    void parseFilename(char *msg, char *filename);
    
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
    bool transmitFile(int socket, char *filename);

    /** @brief Displays the Client menu and gets user selection.
     *  
     *  This function displays all available Client commands and waits for a
     *  user selection. Selections are tested for validity and matched to a
     *  numerical representation.
     *
     *  @param void
     *  @return opt - Integer value representing the command selected by user
     */
    int serverMenu(void);

#endif /* FTP_SERVER_LIB_H */

