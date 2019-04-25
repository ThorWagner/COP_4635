CFLAGS = -g -Wall -Wextra
LFLAGS = -lpthread -lrt
CC = gcc

all : ftpServer ftpClient

ftpServer : server/ftpServer.o
	$(CC) $(CFLAGS) -o server/server server/ftpServer.o $(LFLAGS)

ftpClient : client/ftpClient.o
	$(CC) $(CFLAGS) -o client/client client/ftpClient.o

server/ftpServer.o : server/ftpServer.c server/ftpServer.h
client/ftpClient.o : client/ftpClient.c client/ftpClient.h

.PHONY : clean
clean :
	rm server/server server/ftpServer.o client/client client/ftpClient.o
