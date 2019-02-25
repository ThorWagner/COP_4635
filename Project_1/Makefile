CFLAGS = -g -Wall
CC = gcc

all : httpServer httpClient

httpServer : httpServer.o httpShared.o
	$(CC) $(CFLAGS) -o server httpServer.o httpShared.o

httpClient : httpClient.o
	$(CC) $(CFLAGS) -o client httpClient.o httpShared.o

httpServer.o : httpServer.c httpShared.h
httpClient.o : httpClient.c httpShared.h
httpShared.o : httpShared.c httpShared.h

.PHONY : clean
clean :
	rm server client httpServer.o httpClient.o httpShared.o

