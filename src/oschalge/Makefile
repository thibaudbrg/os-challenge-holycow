CC=gcc

LDLIBS += -lcrypto
CFLAGS += -Ofast

all: server
server: server.o hashtable.o request.o

hashtable.o: hashtable.c hashtable.h request.h
request.o: request.c request.h
server.o: server.c messages.h hashtable.h request.h