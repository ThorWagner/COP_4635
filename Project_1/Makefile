CFLAGS = -g -Wall
CC = gcc

httpServer : httpServer.o
	$(CC) $(CFLAGS) -o server httpServer.o

httpServer.o : httpServer.c httpShared.h

.PHONY : clean
clean :
	rm server httpServer.o
