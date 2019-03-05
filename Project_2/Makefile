CFLAGS = -g -Wall -Wextra
CC = gcc

all : chatServer chatClient

chatServer : chatServer.o
	$(CC) $(CFLAGS) -o server chatServer.c

chatClient : chatClient.o
	$(CC) $(CFLAGS) -o client chatClient.c

chatServer.o : chatServer.c chatShared.h
chatClient.o : chatClient.c chatShared.h

.PHONY : clean
clean :
	rm server chatServer.o client chatClient.o
