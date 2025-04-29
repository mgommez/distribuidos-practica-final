CC = gcc
CFLAGS = -Wall -g
PFLAGS = -fPIC
LDFLAGS = -shared -Wl,-soname,libclaves.so

all :: server

server: server.o users.o comm.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f server *.o
