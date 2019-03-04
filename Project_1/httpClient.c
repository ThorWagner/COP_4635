#include "httpShared.h"

int main(int argC, char **argV){

    int clientFD = 0;
    int portNum = 0;
    char opt = 0;
    char file[ARG_SIZE] = {0};
    struct sockaddr_in servAddr;
    
    // Verify user input
    if((argC > 1) && (argC < 4)){

        // Set port number according to user input or default
        setPort(argC, argV, &portNum, 3);
        printf("Client started on port: %d\n\n", portNum);

        // Extract filename from user input
        strcpy(file, argV[1]);

        while(1){

            // Initialize Server port file descriptor and address
            initClient(&clientFD, &servAddr, portNum);

            clientIntercom(clientFD, &servAddr, portNum, file);
            close(clientFD);
            
            memset(file, 0, ARG_SIZE);
            printf("Would you like to make another request? [y/n]: ");
            scanf("\n\n%c", &opt);
            if((opt == 'n') || (opt == 'N'))
                break;
            printf("Enter the file to request: ");
            scanf("\n\n%s", file);

        }

    }
    else
        printf("\nProper usage:\n\n"
            "./client [file] [port]\n\n"
            "\t*Ensure that [port] is between 60000 and 60099.\n\n"
            "---------- OR ----------\n\n"
            "./client [file]\n\n"
            "\t*Server port will default to 60032.\n\n");

    close(clientFD);

    return 0;

}

