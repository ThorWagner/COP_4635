CFLAGS = -g -Wall -Wextra
LFLAGS = -lpthread -lrt
CC = gcc

serverObjs = server/ftpServer.o server/ftpServerLib.o
clientObjs = client/ftpClient.o client/ftpClientLib.o

all : ftpServer ftpClient

ftpServer : $(serverObjs)
	$(CC) $(CFLAGS) -o server/server $(serverObjs) $(LFLAGS)

ftpClient : $(clientObjs)
	$(CC) $(CFLAGS) -o client/client $(clientObjs)

server/ftpServer.o : server/ftpServer.c include/ftpServerLib.h include/global.h
server/ftpServerLib.o : server/ftpServerLib.c include/ftpServerLib.h include/global.h
client/ftpClient.o : client/ftpClient.c include/ftpClientLib.h
client/ftpClientLib.o : client/ftpClientLib.c include/ftpClientLib.h

.PHONY : clean
clean :
	rm server/server $(serverObjs) client/client $(clientObjs)
