CFLAGS = -g -Wall -Wextra
CC = gcc

all : ftpServer ftpClient

ftpServer : ftpServer.o
	$(CC) $(CFLAGS) -o server ftpServer.o

ftpClient : ftpClient.o
	$(CC) $(CFLAGS) -o client ftpClient.o

ftpServer.o : ftpServer.c ftpServer.h
ftpClient.o : ftpClient.c ftpClient.h

.PHONY : clean
clean :
	rm server ftpServer.o client ftpClient.o
