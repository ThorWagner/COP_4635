#include <signal.h>
#include "httpShared.h"

int main(int argC, char **argV){

    int serverFD = 0;
    int portNum = 0;
    int status = 0;
    struct sockaddr_in servAddr;

    // Verify user input
    if(argC < 3){

        // Set port number according to user input or default
        setPort(argC, argV, &portNum);

        // Initialize Server port file descriptor and address
        initServer(&serverFD, &servAddr, portNum);

        // Listen for incoming Client connections
        status = listen(serverFD, WAIT_SIZE);
        if(status < 0){

            fprintf(stderr, "Failed to connect.\n\n");
            close(serverFD);
            exit(EXIT_FAILURE);

        }

        // Handle signal interrupt [^C]
        signal(SIGINT, sigintHandler);

        // Accept Client connections and respond
        while(1){

            serverIntercom(serverFD, &servAddr);

        }

    }
    else
        printf("\nProper usage:\n\n"
            "./server <port>\n\n"
            "\t*Ensure that <port> is between 60000 and 60099.\n\n"
            "---------- OR ----------\n\n"
            "./server\n\n"
            "\t*Server port will default to 60032.\n\n");

    return 0;

}

