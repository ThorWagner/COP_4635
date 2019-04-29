/** @file ftpClientLib.h
 *  @brief Client specific prerequisites and functionalities
 *
 *  This file the prototyping and static definitions used to initialize and
 *  run the FTP Client with persistant communication to the FTP Server and
 *  continuous user input.
 *
 *  @author Michael Wagner
 *  @date 04/28/2019
 *  @info Course COP 4635
 *  @bug No known bugs.
 */

#ifndef FTP_CLIENT_LIB_H
#define FTP_CLIENT_LIB_H

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

    /* Socket Library */
    #include <sys/socket.h>     // Needed for socket() and connect()

    /* Internet Protocols Library */
    #include <arpa/inet.h>      // Needed for struct sockaddr_in

    /* File Stat Library */
    #include <sys/stat.h>       // Needed for struct stat and stat()

    /* File Transfer Library */
    #include <sys/sendfile.h>   // Needed for sendfile()

    /* File Control Library */
    #include <fcntl.h>          // Needed for open()

    /* -- Global Definitions -- */

    // Defines the default port for FTP communication
    #define DEF_PORT 8080

    // Defines the maximum size for command string
    #define COMM_SIZE 15

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
    int setPort(int argC, char **argV);

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
    void initClient(int *newSock, struct sockaddr_in *address, int portNum);

    /** @brief Displays the Client menu and gets user selection.
     *  
     *  This function displays all available Client commands and waits for a
     *  user selection. Selections are tested for validity and matched to a
     *  numerical representation.
     *
     *  @param void
     *  @return opt - Integer value representing the command selected by user
     */
    int clientMenu(void);

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

#endif /* FTP_CLIENT_LIB_H */

