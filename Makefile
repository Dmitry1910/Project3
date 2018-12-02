CC = gcc
CFLAGS = -D_REENTRANT
LDFLAGS = -lpthread -pthread

web_server: server.c util.o 
	${CC} -o web_server server.c util.o ${LDFLAGS}

util.o: util.c
	${CC} -c util.c

clean:
	rm web_server util.o
