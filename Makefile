CC = gcc
CFLAGS = -Wall -g -I/usr/include/tirpc

SERVER_OBJS = server.o users.o comm.o log_rpc_clnt.o log_rpc_xdr.o
SERVER_RPC_OBJS = server-rpc.o log_rpc_svc.o log_rpc_xdr.o

all :: server server-rpc

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -ltirpc -o $@

server-rpc: $(SERVER_RPC_OBJS)
	$(CC) $(CFLAGS) $^ -ltirpc -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f server server-rpc *.o
