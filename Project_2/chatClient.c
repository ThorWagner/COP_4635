#include "chatShared.h"

void initClient(int *socketFD, struct sockaddr_in *serverAddr);
void sendRecv(int i, int socketFD);

int main(void){

    int i = 0;
    int socketFD = 0;
    int maxFD = 0;
    int current = 0;
    int opt = 99;
    char user[PARAM_SIZE] = {0};
    char pass[PARAM_SIZE] = {0};
    char confirm[PARAM_SIZE] = {0};
    char msg[BUFFER_SIZE] = {0};
    struct sockaddr_in serverAddr;
    fd_set master;
    fd_set listFD;

    initClient(&socketFD, &serverAddr);
    FD_ZERO(&master);
    FD_ZERO(&listFD);
    FD_SET(0, &master);
    FD_SET(socketFD, &master);
    maxFD = socketFD;

    printf("\n\n");

    do{

        // Initial splash screen
        printf("-=| WELCOME TO THE CHAT ROOM |=-\n\n"
            "1: Register\n"
            "2: Login\n"
            "0: Exit\n\n"
            "Enter an option: ");
        scanf("\n\n%d", &opt);
        printf("\n\n");

        switch(opt){

            // Register new user
            case 1:
                printf("-=| REGISTER |=-\n\n");

                // Get new username
                do{

                    memset(user, 0, PARAM_SIZE);
                    memset(confirm, 0, PARAM_SIZE);

                    printf("Enter a username: ");
                    scanf("\n\n%s", user);
                    printf("Confirm username: ");
                    scanf("\n\n%s", confirm);

                    if(strcmp(user, confirm) != 0)
                        printf("The usernames do not match.\n");

                }while(strcmp(user, confirm) != 0);

                // Get new password
                do{

                    memset(pass, 0, PARAM_SIZE);
                    memset(confirm, 0, PARAM_SIZE);

                    printf("Enter a password: ");
                    scanf("\n\n%s", pass);
                    printf("Confirm password: ");
                    scanf("\n\n%s", confirm);

                    if(strcmp(pass, confirm) != 0)
                        printf("The passwords do not match.\n");

                }while(strcmp(pass, confirm) != 0);

                printf("\n\n");

                memset(msg, 0, BUFFER_SIZE);
                sprintf(msg, "1-%s:%s", user, pass);
                send(socketFD, msg, strlen(msg), 0);

                break;

            case 2:
                printf("-=| USER LOGIN |=-\n\n");

                memset(user, 0, PARAM_SIZE);
                memset(pass, 0, PARAM_SIZE);
                printf("Enter username: ");
                scanf("%s", user);
                printf("Enter password: ");
                scanf("%s", pass);

                printf("\n\n");

                memset(msg, 0, BUFFER_SIZE);
                sprintf(msg, "2-%s:%s", user, pass);
                send(socketFD, msg, strlen(msg), 0);

                memset(msg, 0, BUFFER_SIZE);
                recv(socketFD, msg, BUFFER_SIZE, 0);

                if(strcmp(msg, "1") == 0){

                    opt = 99;
                    do{

                        printf("-=| MAIN MENU|=-\n\n"
                            "1: View number of users currently online.\n"
                            "2: Enter the group chat.\n"
                            "3: Enter a private chat.\n"
                            "4: View chat history.\n"
                            "5: File transfer.\n"
                            "6: Change password.\n"
                            "7: Logout\n"
                            "8: Administration\n"
                            "0: Return to the login screen\n\n"
                            "Enter an option: ");
                        scanf("\n\n%d", &opt);

                        printf("\n\n");

                        switch(opt){

                            case 1:
                                break;

                            case 2:
                                printf("-=| GROUP CHAT |=-\n\n");

                                while(1){

                                    listFD = master;
                                    current = select(maxFD + 1, &listFD, NULL,
                                        NULL, NULL);
                                    if(current == -1){

                                        perror("Select");
                                        exit(EXIT_FAILURE);

                                    }

                                    for(i = 0; i <= maxFD; i++)
                                        if(FD_ISSET(i, &listFD) != 0)
                                            sendRecv(i, socketFD);

                                }

                                break;

                            case 7:
                                printf("Logging out...");
                                break;

                            case 0:
                                printf("Returning to the logic screen...");
                                break;

                            default:
                                printf("Select a valid option.\n\n\n");
                                break;

                        }

                    }while((opt != 0) && (opt != 7));

                }
                else
                    printf("Invalid username or password.\n\n\n");

                break;

            case 0:
                printf("Exiting...\n\n");
                break;

            default:
                printf("\nSelect a valid option\n\n");
                break;

        }

    }while(opt != 0);

    close(socketFD);

    return 0;

}

void initClient(int *socketFD, struct sockaddr_in *serverAddr){

    int status = 0;
    
    *socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketFD == -1){

        perror("Socket");
        exit(EXIT_FAILURE);

    }

    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(DEF_PORT);
    serverAddr->sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr->sin_zero, 0, sizeof(serverAddr->sin_zero));

    status = connect(*socketFD, (struct sockaddr *)serverAddr,
        sizeof(struct sockaddr));
    if(status == -1){

        perror("Connection");
        exit(EXIT_FAILURE);

    }

    return;

}

void sendRecv(int i, int socketFD){

    char temp[BUFFER_SIZE] = {0};
    char inBuff[BUFFER_SIZE] = {0};
    char outBuff[BUFFER_SIZE] = {0};
//    int bytesIn = 0;

    if(i == 0){

        scanf("\n\n%[^\n]", temp);
        strcpy(outBuff, "22-");
        strcat(outBuff, temp);
        if(strcmp(outBuff, "exit") == 0)
            exit(EXIT_SUCCESS);
        else
            send(socketFD, outBuff, strlen(outBuff), 0);

    }
    else{

        recv(socketFD, inBuff, BUFFER_SIZE, 0);
        printf("%s\n", inBuff);
        fflush(stdout);

    }

    return;

}

