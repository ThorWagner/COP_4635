/** @file ftpServer.c
 *  @brief Main Server program driver
 *
 *  This file contains the main program body for the FTP Server. It relies on
 *  function definitions and static definitions made in 'ftpServerLib.c' and
 *  'ftpServerLib.h' as well as global variable declared in 'global.h'. A
 *  persistent TCP connection is formed with each FTP Client via independent
 *  to allow for simultaneous Client requests.
 *
 *  @author Michael Wagner
 *  @date 04/28/2019
 *  @info Course COP 4635
 *  @bug No known bugs.
 */

/* -- Includes -- */

/* FTP Server header */
#include "../include/ftpServerLib.h"

/* Global Variable header */
#include "../include/global.h"

/* -- Global Variables -- */

// Flag to stop threaded handling of clients
extern bool g_running;

// Counter to keep track of active users
extern int g_current;

// Counter to keep track of total visitors
extern int g_all;

/** @brief Main Server program driver
 *  
 *  Primary functionality of the FTP Server program. Attempts to start the
 *  Server on the specified port (or default if none provided) and begin
 *  listening for incoming FTP Client connections. Runs continuously until the
 *  exit command is received, with separate threads handling user input,
 *  connection monitors, and connection handling. Allows the user to 1) Display
 *  the number of active Client connections, 2) Display the total number of
 *  system visitors since startup, 3) Ask for sarcastic help, and 4) Exit the
 *  Server.
 *
 *  @param argC - Provided by the system, indicats the number of arguments
 *  @param argV - Provided by the system, contains all argument strings
 *  @return 0 on exit
 */
int main(int argC, char **argV){

    int servSock = 0;
    int portNum = 0;
    struct sockaddr_in servAddr;
    pthread_t thread;

    // Initialization of global variables
    g_running = true;
    g_current = 0;
    g_all = 0;

    if(argC > 2){

        fprintf(stderr, "\nProper usage: ./server   OR   ./server [port]\n\n");
        return 1;

    }

    portNum = setPort(argC, argV);

    initServer(&servSock, &servAddr, portNum);

    int opt = 0;
    bool running = true;
    pthread_create(&thread, 0, threadedMonitor, (void *)&servSock);
    do{

        opt = serverMenu();
        switch(opt){

            case 1:
                running = false;
                shutdown(servSock, SHUT_RDWR);
                g_running = false;
                break;

            case 2:
                printf("\nActive users: %d\n", g_current);
                break;

            case 3:
                printf("\nAll system visitors: %d\n", g_all);
                break;

            default:
                printf("\nAn error occured.\n\n");
                break;

        }

    }while(running);

    pthread_join(thread, NULL);
    printf("Shutting down server...\n\n");

    return 0;

}

