CC = gcc
CFLAGS = -g
RPCGEN_FLAG = -C

all: conecta4.h client server

client: client.o conecta4_clnt.o conecta4_xdr.o
	$(CC) -o client game.o utils.o client.o conecta4_clnt.o conecta4_xdr.o

server: server.o conecta4_svc.o conecta4_xdr.o
	$(CC) -o server game.o utils.o server.o conecta4_svc.o conecta4_xdr.o

server.o: server.c conecta4.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c conecta4.h
	$(CC) $(CFLAGS) -c client.c

conecta4_svc.o: conecta4_svc.c conecta4.h
	$(CC) $(CFLAGS) -c conecta4_svc.c 
		
conecta4_clnt.o: conecta4_clnt.c conecta4.h
	$(CC) $(CFLAGS) -c conecta4_clnt.c 
	
conecta4_xdr.o: conecta4_xdr.c conecta4.h 
	$(CC) $(CFLAGS) -c conecta4_xdr.c

clean:
	rm -f client client.o server server.o conecta4_clnt.* conecta4_svc.* conecta4_xdr.* conecta4.h

