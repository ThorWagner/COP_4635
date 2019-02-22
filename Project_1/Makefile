CFLAGS = -g -Wall
CC = gcc

all : httpServer httpClient

httpServer : httpServer.o
	$(CC) $(CFLAGS) -o server httpServer.o

httpClient : httpClient.o
	$(CC) $(CFLAGS) -o client httpClient.o

httpServer.o : httpServer.c httpShared.h
httpClient.o : httpClient.c httpShared.h

.PHONY : clean
clean :
	rm server httpServer.o client httpClient.o

