#include "chatShared.h"

void initClient(int *socketFD, struct sockaddr_in *serverAddr);
bool sendRecv(int i, int socketFD, char *currentID);
void updateLog(char *currentID, char *msg);

int main(void){

    int i = 0;
    int socketFD = 0;
    int maxFD = 0;
    int current = 0;
    int opt1 = 0;
    int opt2 = 0;
    int opt3 = 0;
    char user[PARAM_SIZE] = {0};
    char pass[PARAM_SIZE] = {0};
    char confirm[PARAM_SIZE] = {0};
    char currentID[PARAM_SIZE] = {0};
    char old[PARAM_SIZE] = {0};
    char new[PARAM_SIZE] = {0};
    char msg1[BUFFER_SIZE] = {0};
    char msg2[BUFFER_SIZE] = {0};
    char logfile[2 * PARAM_SIZE] = {0};
    bool exitFlag = false;
    FILE *infile = NULL;
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
        opt1 = 99;
        printf("-=| WELCOME TO THE CHAT ROOM |=-\n\n"
            "1: Register\n"
            "2: Login\n"
            "0: Exit\n\n"
            "Enter an option: ");
        scanf("\n\n%d", &opt1);

        switch(opt1){

            // Register new user
            case 1:
                printf("\n\n-=| REGISTER |=-\n\n");

                // Get new username
                do{

                    memset(user, 0, PARAM_SIZE);
                    printf("Enter a username: ");
                    scanf("\n\n%s", user);

                    memset(confirm, 0, PARAM_SIZE);
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

                memset(msg1, 0, BUFFER_SIZE);
                sprintf(msg1, "1-%s:%s", user, pass);
                send(socketFD, msg1, strlen(msg1), 0);

                break;

            case 2:
                printf("\n\n-=| USER LOGIN |=-\n\n");

                if(strlen(currentID) != 0){

                    memset(msg1, 0, BUFFER_SIZE);
                    strcpy(msg1, "1");
                    
                    printf("Welcome back %s!\n", currentID);

                }
                else{
                
                    memset(user, 0, PARAM_SIZE);
                    printf("Enter username: ");
                    scanf("%s", user);

                    memset(pass, 0, PARAM_SIZE);
                    printf("Enter password: ");
                    scanf("%s", pass);

                    memset(msg1, 0, BUFFER_SIZE);
                    sprintf(msg1, "2-%s:%s", user, pass);
                    send(socketFD, msg1, strlen(msg1), 0);

                    memset(msg1, 0, BUFFER_SIZE);
                    recv(socketFD, msg1, BUFFER_SIZE, 0);

                }

                if(strcmp(msg1, "1") == 0){

                    strcpy(currentID, user);
                    
                    do{

                        // Main menu after login
                        opt2 = 99;
                        printf("\n\n-=| MAIN MENU|=-\n\n"
                            "1: View number of users currently online\n"
                            "2: Enter the group chat\n"
                            "3: Enter a private chat\n"
                            "4: View chat history\n"
                            "5: File transfer\n"
                            "6: Change password\n"
                            "7: Logout\n"
                            "8: Administration\n"
                            "0: Return to the login screen\n\n"
                            "Enter an option: ");
                        scanf("\n\n%d", &opt2);

                        switch(opt2){

                            case 1:
                                printf("\n\n-=| VIEW NUMBER OF USERS CURRENTLY "
                                    "ONLINE |=-\n\n");
                                
                                memset(msg1, 0, BUFFER_SIZE);
                                sprintf(msg1, "21-users");
                                send(socketFD, msg1, strlen(msg1), 0);

                                memset(msg1, 0, BUFFER_SIZE);
                                recv(socketFD, msg1, BUFFER_SIZE, 0);

                                printf("Users currently online: %s\n", msg1);

                                break;

                            case 2:
                                printf("\n\n-=| GROUP CHAT |=-\n\n"
                                    "Type 'ESC' at any time to return to the "
                                    "main menu.\n\n");

                                while(exitFlag == false){

                                    listFD = master;
                                    current = select(maxFD + 1, &listFD, NULL,
                                        NULL, NULL);
                                    if(current == -1){

                                        perror("Select");
                                        exit(EXIT_FAILURE);

                                    }

                                    for(i = 0; i <= maxFD; i++){

                                        exitFlag = false;

                                        if(FD_ISSET(i, &listFD) != 0)
                                            exitFlag = sendRecv(i, socketFD,
                                                currentID);

                                        if(exitFlag == true)
                                            break;

                                    }

                                }

                                break;

                            case 3:
                                printf("\n\n-=| PRIVATE CHAT |=-\n\n");

                                memset(user, 0, PARAM_SIZE);
                                printf("Enter the recipient username: ");
                                scanf("\n\n%s", user);

                                memset(msg1, 0, PARAM_SIZE);
                                printf("Enter message: ");
                                scanf("\n\n%[^\n]", msg1);

                                memset(msg2, 0, PARAM_SIZE);
                                sprintf(msg2, "23-%s:(private)%s: ", user,
                                    currentID);
                                strcat(msg2, msg1);
                                send(socketFD, msg2, strlen(msg2), 0);

                                updateLog(currentID, strcat("(private)Me: ",
                                    msg1));

                                break;

                            case 4:
                                do{

                                    opt3 = 99;

                                    printf("\n\n-=| CHAT HISTORY |=-\n\n"
                                        "1: Group Chat\n"
                                        "2: Private Chat\n"
                                        "0: Back\n\n"
                                        "Enter an option: ");
                                    scanf("\n\n%d", &opt3);


                                    switch(opt3){

                                        case 1:
                                            printf("\n-=| GROUP CHAT HISTORY "
                                                "|=-\n\n");

                                            sprintf(logfile, "%s.dat",
                                                currentID);
                                            printf("%s\n", logfile);
                                            infile = fopen(logfile, "r");

                                            if(infile != NULL){
                                            
                                                while(fgets(msg1,
                                                    3 * PARAM_SIZE,
                                                    infile) != NULL){

                                                    if(strncmp(msg1,
                                                        "(private)", 9) != 0)
                                                        printf("%s", msg1);

                                                }

                                            }
                                            else
                                                printf("Log file unavailable.\n");

                                            fclose(infile);

                                            break;

                                        case 2:
                                            printf("\n-=| PRIVATE CHAT HISTORY "
                                                "|=-\n\n");

                                            sprintf(logfile, "%s.dat",
                                                currentID);
                                            infile = fopen(logfile, "r");

                                            if(infile != NULL){

                                                while(fgets(msg1,
                                                    3 * PARAM_SIZE,
                                                    infile) != NULL){

                                                    if(strncmp(msg1,
                                                        "(private)", 9) == 0)
                                                    printf("%s", msg1);

                                                }

                                            }
                                            else
                                                printf("Log file unavailable\n");

                                            fclose(infile);

                                            break;

                                        case 0:
                                            printf("\n\n-=| RETURNING TO MAIN "
                                                "MENU |=-\n\n\n");
                                            break;

                                    }

                                }while(opt3 != 0);

                                break;

                            case 5:
                                opt3 = 99;

                                do{

                                    printf("\n\n-=| FILE TRANSFER |=-\n\n"
                                        "1: Select file\n"
                                        "2: Select object\n"
                                        "0: Back\n\n");
                                    scanf("\n\n%d", &opt3);

                                    switch(opt3){

                                        case 1:
                                            break;

                                        case 2:
                                            break;

                                        case 0:
                                            printf("\n\n-=| RETURNING TO MAIN "
                                                "MENU |=-\n\n\n");
                                            break;

                                        default:
                                            printf("\nEnter a valid option."
                                                "\n\n\n");
                                            break;

                                    }

                                }while(opt3 != 0);

                                break;

                            case 6:
                                printf("\n\n-=| CHANGE PASSWORD |=-\n\n");

                                // Check that user has proper credentials
                                do{

                                    // Get current password
                                    memset(old, 0, PARAM_SIZE);
                                    printf("Enter old password: ");
                                    scanf("\n\n%s", old);

                                    // Send username and current password
                                    memset(msg1, 0, BUFFER_SIZE);
                                    sprintf(msg1, "26-%s:%s", currentID, old);
                                    send(socketFD, msg1, strlen(msg1), 0);

                                    // Get Server response
                                    memset(msg1, 0, BUFFER_SIZE);
                                    recv(socketFD, msg1, BUFFER_SIZE, 0);

                                    // Username and password combo incorrect
                                    if(strcmp(msg1, "1") != 0)
                                        printf("Incorrect password.\n");

                                }while(strcmp(msg1, "1") != 0);

                                // Get new password
                                memset(new, 0, PARAM_SIZE);
                                printf("Enter new password: ");
                                scanf("\n\n%s", new);

                                // Confirm new password
                                do{

                                    // Get new password again
                                    memset(confirm, 0, PARAM_SIZE);
                                    printf("Confirm new password: ");
                                    scanf("\n\n%s", confirm);

                                    // Second attempt was different
                                    if(strcmp(new, confirm) != 0)
                                        printf("The new passwords do not match."
                                            "\n");

                                }while(strcmp(new, confirm) != 0);

                                // Send username and new password
                                memset(msg2, 0, BUFFER_SIZE);
                                sprintf(msg2, "%s:%s", currentID, new);
                                send(socketFD, msg2, strlen(msg2), 0);

                                break;

                            case 7:
                                printf("\n\n-=| LOGGING OUT |=-\n\n\n");
                                memset(currentID, 0, PARAM_SIZE);
                                break;

                            case 8:
                                printf("\n\n-=| ADMINISTRATOR |=-\n\n");
                                break;

                            case 0:
                                printf("\n\n-=| RETURNING TO LOGIN SCREEN |=-"
                                    "\n\n\n");
                                break;

                            default:
                                printf("\nSelect a valid option.\n\n\n");
                                break;

                        }

                    }while((opt2 != 0) && (opt2 != 7));

                }
                else
                    printf("Invalid username or password.\n\n\n");

                break;

            case 0:
                printf("\n\n-=| EXITING |=-\n\n\n");
                break;

            default:
                printf("\nSelect a valid option\n\n\n");
                break;

        }

    }while(opt1 != 0);

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

bool sendRecv(int i, int socketFD, char *currentID){

    char temp[BUFFER_SIZE] = {0};
    char inBuff[BUFFER_SIZE] = {0};
    char outBuff[BUFFER_SIZE] = {0};
    bool exitFlag = false;

    if(i == 0){

        scanf("\n\n%[^\n]", temp);
        sprintf(outBuff, "22-%s: ", currentID);
        strcat(outBuff, temp);
        if(strcmp(temp, "ESC") == 0)
            exitFlag = true;
        else{

            send(socketFD, outBuff, strlen(outBuff), 0);

            updateLog(currentID, temp);

        }

    }
    else{

        recv(socketFD, inBuff, BUFFER_SIZE, 0);
        printf("%s\n", inBuff);
        fflush(stdout);

        updateLog(currentID, inBuff);

    }

    return exitFlag;

}

void updateLog(char *currentID, char *msg){

    char filename[2 * PARAM_SIZE] = {0};
    FILE *outfile = NULL;

    sprintf(filename, "%s.dat", currentID);

    outfile = fopen(filename, "a");
    fprintf(outfile, "%s\n", msg);
    fclose(outfile);

    return;

}

