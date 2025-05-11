#ifndef DISTRIBUIDOS_EJ2_COMM_H
#define DISTRIBUIDOS_EJ2_COMM_H

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

int     serverSocket ( unsigned int addr, int port, int type ) ;
int     serverAccept ( int sd ) ;
int     closeSocket  ( int sd ) ;

int     sendMessage  ( int socket, char *buffer, int len );


int     getClientIp  (int socket, char *ip_client);

ssize_t writeLine    ( int fd, char *buffer ) ;
ssize_t readLine     ( int fd, char *buffer, size_t n );

#endif //DISTRIBUIDOS_EJ2_COMM_H
