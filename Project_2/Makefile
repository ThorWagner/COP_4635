CFLAGS = -g -Wall -Wextra
CC = gcc

all : chatServer chatClient

chatServer :
	$(CC) $(CFLAGS) -o server chatServer.c

chatClient :
	$(CC) $(CFLAGS) -o client chatClient.c

.PHONY : clean
clean :
	rm server client
