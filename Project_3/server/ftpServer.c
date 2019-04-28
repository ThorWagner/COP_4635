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
//volatile int g_current = 0;

// Counter to keep track of total system visitors
//volatile int g_all = 0;

int main(int argC, char **argV){

    int servSock = 0;
    int portNum = 0;
//    int current = 0;
//    int all = 0;
    struct sockaddr_in servAddr;
//    conn_t *connection = NULL;
    pthread_t thread;

    // Initialization of global variables
    g_running = true;
    g_current = 0;
    g_all = 0;
//    g_current = malloc(sizeof(int));
//    *g_current = 0;
//    g_all = malloc(sizeof(int));
//    *g_all = 0;

    if(argC > 2){

        fprintf(stderr, "\nProper usage: ./server   OR   ./server [port]\n\n");
        return 1;

    }

    portNum = setPort(argC, argV);

    initServer(&servSock, &servAddr, portNum);

    //signal(SIGINT, sigintHandler);

/*    while(1){

        connectClient(servSock, connection, &thread);

    }
*/

/*    int opt = 0;
    bool running = true;
    pid_t pid = fork();
    if((int)pid == 0){

        while(1)
            connectClient(servSock, connection, &thread);

    }
    else{

        while(running){

            opt = serverMenu();
            switch(opt){

                case 1:
                    printf("\nShutting down server...\n\n");
                    kill(pid, SIGKILL);
                    running = false;
                    break;

                case 2:
                    printf("\nActive users: %d\n", current);
                    break;

                case 3:
                    printf("\nAll system visitors: %d\n", all);
                    break;

                default:
                    printf("\nAn error occured.\n\n");
                    break;

            }

        }

    }
*/

    int opt = 0;
    bool running = true;
    pthread_create(&thread, 0, threadedMonitor, (void *)&servSock);
    do{

        opt = serverMenu();
        switch(opt){

            case 1:
                running = false;
                //close(servSock);
                shutdown(servSock, SHUT_RDWR);
                //pthread_cancel(thread);
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

    //close(servSock);

    return 0;

}

