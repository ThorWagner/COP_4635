CFLAGS = -g -Wall -Wextra
CC = gcc

all : chatServer chatClient

chatServer : chatServer.o chatShared.o
	$(CC) $(CFLAGS) -o server chatServer.o chatShared.o

chatClient : chatClient.o chatShared.o
	$(CC) $(CFLAGS) -o client chatClient.o chatShared.o

chatServer.o : chatServer.c chatShared.h
chatClient.o : chatClient.c chatShared.h
chatShared.o : chatShared.c chatShared.h

.PHONY : clean
clean :
	rm server chatServer.o client chatClient.o chatShared.o
