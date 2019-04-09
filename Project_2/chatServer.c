/*
 * Michael Wagner
 * COP 4635 - Project 2
 */

#include <signal.h>
#include "chatShared.h"

#define FILENAME "database.dat"

typedef struct cred{
    char user[PARAM_SIZE];
    char pass[PARAM_SIZE];
    struct cred *next;
    bool online;
    bool banned;
    int port;
}cred;

void initServer(int *serverFD, struct sockaddr_in *serverAddr);
void broadcast(int i, int j, int serverFD, int bytesIn, char *inBuff,
    fd_set *master);
void sendRecv(int i, int serverFD, int maxFD, fd_set *master, int *numClients);
void connectClient(int serverFD, int *maxFD, struct sockaddr_in *clientAddr,
    fd_set *master, int *numClients);
void clearCredentials(cred *node);
void buildDatabase(cred *head, char *filename);
void destroyDatabase(cred *head);
void sigintHandler(int sig);
bool checkCredentials(cred *head, char *combined);
void addCredentials(cred *head, char *combined);
void updateCredentials(cred *head, char *combined);
void storeDatabase(cred *head, char *filename);
void setOnline(cred *head, char *combined, int i);
int numOnline(cred *head);
void setOffline(cred *head, int i);
int getUserPort(cred *head, char *recipient);
//bool checkStatus(cred *head, char *combined);
void checkUserStatus(cred *head, char *user, bool *found, bool *online,
    bool *banned);
void banUser(cred *head, char *user);

// Global Variables
cred *g_head = NULL;

int main(void){

    int i = 0;
    int serverFD = 0;
    int maxFD = 0;
    int status = 0;
    int numClients = 0;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    fd_set master;
    fd_set temp;
    //cred *head = NULL;

    // Read in user database
    g_head = malloc(sizeof(cred));
    clearCredentials(g_head);
    buildDatabase(g_head, FILENAME);

    // Initialize file descriptor lists as empty
    FD_ZERO(&master);
    FD_ZERO(&temp);

    // Initialize Server port file descriptor and address
    initServer(&serverFD, &serverAddr);

    // Add Server 'listening' port to file descriptor master list
    FD_SET(serverFD, &master);

    // Handle signal interrupt [^C]
    signal(SIGINT, sigintHandler);

    maxFD = serverFD;
    while(1){

        // Backup master list to expendable temp list
        temp = master;

        // Wait for Client activity
        status = select(maxFD + 1, &temp, NULL, NULL, NULL);
        if(status == -1)
            showError("Failed to select active client.");

        for(i = 0; i <= maxFD; i++){

            if(FD_ISSET(i, &temp)){

                if(i == serverFD)
                    connectClient(serverFD, &maxFD, &clientAddr, &master,
                        &numClients);
                else
                    sendRecv(i, serverFD, maxFD, &master, &numClients);

            }

            printf("Number of Clients: %d\n\n", numOnline(g_head));

        }

    }

    return 0;

}

void initServer(int *serverFD, struct sockaddr_in *serverAddr){

    int addrLen = 0;
    int status = 0;
    int opt = 1;

    addrLen = sizeof(struct sockaddr_in);

    // Create Server socket file descriptor and address
    *serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if(*serverFD == -1)
        showError("Failed to create socket.");

    memset(serverAddr, 0, addrLen);
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_addr.s_addr = INADDR_ANY;
    serverAddr->sin_port = htons(DEF_PORT);

    // Allow the Server socket to be reused
    status = setsockopt(*serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if(status == -1)
        showError("Failed to configure socket.");

    // Bind the Server socket
    status = bind(*serverFD, (struct sockaddr *)serverAddr, addrLen);
    if(status == -1){

        close(*serverFD);
        showError("Failed to bind socket.");

    }

    // Output success message
    printf("\nServer started on port:: %d\n\n", DEF_PORT);
    fflush(stdout);

    // Listen for incoming Client connections
    status = listen(*serverFD, WAIT_SIZE);
    if(status == -1){

        close(*serverFD);
        showError("Failed to connect.");

    }

    return;

}

void broadcast(int i, int j, int serverFD, int bytesIn, char *inBuff,
    fd_set *master){

    int status = 0;

    if(FD_ISSET(j, master) != 0){

        if((j != serverFD) && (j != i)){

            status = send(j, inBuff, bytesIn, 0);
            if(status == -1)
                fprintf(stderr, "Failed to send message");

        }

    }

    return;

}

void sendRecv(int i, int serverFD, int maxFD, fd_set *master, int *numClients){

    int j = 0;
    int bytesIn = 0;
    int msgCode = 0;
    int privatePort = 0;
    char inBuff[BUFFER_SIZE] = {0};
    char outBuff[BUFFER_SIZE] = {0};
    char recipient[PARAM_SIZE] = {0};
    char privateMsg[BUFFER_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    bool valid = false;
    bool found = false;
    //bool loggedIn = false;
    bool online = false;
    bool banned = false;

    bytesIn = recv(i, inBuff, BUFFER_SIZE, 0);
    if(bytesIn <= 0){

        if(bytesIn == 0){

            printf("Socket %d disconnected\n", i);

            *numClients -= 1;

        }
        else
            fprintf(stderr, "\nError receiving message.");

        setOffline(g_head, i);
        close(i);
        FD_CLR(i, master);

    }
    else{

        sscanf(inBuff, "%d-%[^\n]", &msgCode, inBuff);

        switch(msgCode){

            case 1:
                addCredentials(g_head, inBuff);
                storeDatabase(g_head, FILENAME);

                break;

            case 2:
                valid = checkCredentials(g_head, inBuff);
                //checkUserStatus(g_head, inBuff, &found, &online, &banned);
                if(valid == true){

                    checkUserStatus(g_head, inBuff, &found, &online, &banned);
                    if(banned == false){

                        //loggedIn = checkStatus(g_head, inBuff);
                        //if(loggedIn == false){
                        if(online == false){

                            setOnline(g_head, inBuff, i);
                            send(i, "1", 1, 0);

                        }
                        else
                            send(i, "2", 1, 0);

                    }
                    else
                        send(i, "3", 1, 0);

                }
                else
                    send(i, "0", 1, 0);

                break;

            case 21:
                memset(outBuff, 0, BUFFER_SIZE);
                sprintf(outBuff, "%d", *numClients);
                send(i, outBuff, strlen(outBuff), 0);
                break;
            
            case 22:
                for(j = 0; j <= maxFD; j++)
                    broadcast(i, j, serverFD, strlen(inBuff), inBuff, master);

                break;

            case 23:
                token = strtok(inBuff, delim);
                strcpy(recipient, token);
                //token = strtok(NULL, delim);
                strcpy(privateMsg, inBuff + strlen(recipient) + 1);

                privatePort = getUserPort(g_head, recipient);
                if(privatePort > 0)
                    send(privatePort, privateMsg, strlen(privateMsg), 0);
                else
                    send(i, "User unavailable.", 17, 0);


                break;

            case 26:
                valid = checkCredentials(g_head, inBuff);
                //checkUserStatus(g_head, inBuff, &found, &online, &banned);
                if(valid == true){

                    send(i, "1", 1, 0);

                    memset(inBuff, 0, BUFFER_SIZE);
                    recv(i, inBuff, BUFFER_SIZE, 0);

                    updateCredentials(g_head, inBuff);

                    storeDatabase(g_head, FILENAME);

                }
                else
                    send(i, "0", 1, 0);

                break;

            case 28:
                checkUserStatus(g_head, inBuff, &found, &online, &banned);
                if(found == true){

                    banUser(g_head, inBuff);
                    send(i, "1", 1, 0);

                }
                else
                    send(i, "0", 1, 0);

                break;

            default:
                printf("A mistake was made...");
                break;

        }

    }

    return;

}

void connectClient(int serverFD, int *maxFD, struct sockaddr_in *clientAddr,
    fd_set *master, int *numClients){

    int addrLen = 0;
    int clientFD = 0;

    addrLen = sizeof(struct sockaddr_in);
    
    clientFD = accept(serverFD, (struct sockaddr *)clientAddr,
        (socklen_t *)&addrLen);
    if(clientFD == -1)
        showError("Failed to accept client connection");
    else{

        FD_SET(clientFD, master);
        if(clientFD > *maxFD)
            *maxFD = clientFD;

        printf("New connection from %s on port %d\n",
            inet_ntoa(clientAddr->sin_addr), ntohs(clientAddr->sin_port));

        *numClients += 1;

    }

    return;

}

void clearCredentials(cred *node){

    memset(node->user, 0, PARAM_SIZE);
    memset(node->pass, 0, PARAM_SIZE);
    node->next = NULL;
    node->online = false;
    node->banned = false;
    node->port = 0;

    return;

}

void buildDatabase(cred *head, char *filename){

    char buffer[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    FILE *database = NULL;
    cred *current = NULL;

    current = head;

    database = fopen(filename, "r");
    if(database != NULL){

        fscanf(database, "%s", buffer);
        token = strtok(buffer, delim);
        strcpy(current->user, token);
        token = strtok(NULL, delim);
        strcpy(current->pass, token);
        
        while(!feof(database)){

            memset(buffer, 0, 3 * PARAM_SIZE);
            fscanf(database, "%s", buffer);
            if(strlen(buffer) > 0){

                current->next = malloc(sizeof(cred));
                current = current->next;
                clearCredentials(current);

                token = strtok(buffer, delim);
                strcpy(current->user, token);
                token = strtok(NULL, delim);
                strcpy(current->pass, token);

            }

        }

        current = head;
        while(current != NULL){

            printf("[%s]:[%s]\n", current->user, current->pass);
            current = current->next;

        }

    }
    else
        showError("Could not access database.");

    return;

}

void destroyDatabase(cred *head){

    cred *current = NULL;
    cred *prev = NULL;

    current = head;
    while(current != NULL){

        if(current->next != NULL){

            prev = current;
            current = current->next;
            free(prev);

        }
        else{

            free(current);
            current = NULL;

        }

    }

    return;

}

void sigintHandler(int sig){

    // Verify server process is ending
    fprintf(stderr, "\nShutting down server...\n\n");

    // Clean up data
    destroyDatabase(g_head);

    // Return SIGINT
    exit(sig);

}

bool checkCredentials(cred *head, char *combined){

    char username[PARAM_SIZE] = {0};
    char password[PARAM_SIZE] = {0};
    char temp[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    bool found = false;
    cred *current = NULL;

    current = head;
    strcpy(temp, combined);

    token = strtok(temp, delim);
    strcpy(username, token);
    token = strtok(NULL, delim);
    strcpy(password, token);

    while(current != NULL){

        if(strcmp(current->user, username) == 0){

            if(strcmp(current->pass, password) == 0){

                found = true;
                break;

            }

        }

        current = current->next;

    }

    return found;

}

void addCredentials(cred *head, char *combined){

    char username[PARAM_SIZE] = {0};
    char password[PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    cred *current = NULL;
    cred *prev = NULL;

    current = head;

    token = strtok(combined, delim);
    strcpy(username, token);
    token = strtok(NULL, delim);
    strcpy(password, token);

    while(current != NULL){

        prev = current;
        current = current->next;

    }

    prev->next = malloc(sizeof(cred));
    current = prev->next;
    clearCredentials(current);

    strcpy(current->user, username);
    strcpy(current->pass, password);

    return;

}

void updateCredentials(cred *head, char *combined){

    char username[PARAM_SIZE] = {0};
    char password[PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    cred *current = NULL;

    current = head;

    token = strtok(combined, delim);
    strcpy(username, token);
    token = strtok(NULL, delim);
    strcpy(password, token);

    while(current != NULL){

        if(strcmp(current->user, username) == 0){

            strcpy(current->pass, password);
            break;

        }

        current = current->next;

    }

    return;

}

void storeDatabase(cred *head, char *filename){

    FILE *database = NULL;
    cred *current = NULL;

    database = fopen(filename, "w");

    current = head;

    while(current != NULL){

        fprintf(database, "%s:%s\n", current->user, current->pass);

        current = current->next;

    }

    fclose(database);

    return;

}

void setOnline(cred *head, char *combined, int i){

    char username[PARAM_SIZE] = {0};
    char temp[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    cred *current = NULL;

    current = head;
    strcpy(temp, combined);

    token = strtok(temp, delim);
    strcpy(username, token);

    while(current != NULL){

        if(strcmp(current->user, username) == 0)
            break;

        current = current->next;

    }

    current->online = true;
    current->port = i;

    return;

}

int numOnline(cred *head){

    int i = 0;
    cred *current = NULL;

    current = head;
    while(current != NULL){

        if(current->online == true)
            i++;

        current = current->next;

    }

    return i;

}

void setOffline(cred *head, int i){

    cred *current = NULL;

    current = head;
    while(current != NULL){

        if(current->port == i){

            current->online = false;
            current->port = 0;
            break;

        }

        current = current->next;

    }

    return;

}

int getUserPort(cred *head, char *recipient){

    int port = 0;
    cred *current = NULL;

    current = head;
    while(current != NULL){

        if(strcmp(current->user, recipient) == 0){

            if(current->online == true)
                port = current->port;

            break;

        }

        current = current->next;

    }

    return port;

}

/*bool checkStatus(cred *head, char *combined){

    char username[PARAM_SIZE] = {0};
    char temp[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    bool loggedIn = false;
    cred *current = NULL;

    current = head;
    strcpy(temp, combined);

    token = strtok(temp, delim);
    strcpy(username, token);

    while(current != NULL){

        if(strcmp(current->user, username) == 0){

            if(current->online == true){

                loggedIn = true;
                break;

            }

        }

        current = current->next;

    }

    return loggedIn;

}*/

void checkUserStatus(cred *head, char *user, bool *found, bool *online,
    bool *banned){

    char username[PARAM_SIZE] = {0};
    char temp[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    cred *current = NULL;

    current = head;
    strcpy(temp, user);

    token = strtok(temp, delim);
    strcpy(username, token);

    *found = false;
    *online = false;
    *banned = false;

    while(current != NULL){

        if(strcmp(current->user, username) == 0){

            *found = true;
            *online = current->online;
            *banned = current->banned;

        }

        current = current->next;

    }

    return;

}

void banUser(cred *head, char *user){

    char username[PARAM_SIZE] = {0};
    char temp[3 * PARAM_SIZE] = {0};
    char *token = NULL;
    char *delim = ":";
    cred *current = NULL;

    current = head;
    strcpy(temp, user);

    token = strtok(temp, delim);
    strcpy(username, token);

    while(current != NULL){

        if(strcmp(current->user, username) == 0){

            current->banned = true;
            break;

        }

        current = current->next;

    }

    return;

}

