#pragma once

#include "socket.h"

#define SERVER_BACKLOG 10
#define SERVER_MINSOCKSLEN 16

typedef struct sockserver
{
    int ip_version;  // IPv4, IPv6
    int socket_type; // TCP, UDP
    char ip_address[64];
    char port[5];

    int sockfd;             // Main socket file descriptor
    int num_conn;           // Number of connections. TCP only
    struct pollfd *sockets; // Array of socket file descriptors
    size_t socket_cap;      // Size of sockets array, default 16
    int pollcount;          // Number of events from last poll call
} sockserver;

sockserver *serverinit(int ipv, int socktype, const char *ipaddr, const char *port);
void servershutdown(sockserver *server);
int serveraccept(sockserver *server);

int serversend(sockserver *server, int id, const char *data, int len);
int serverrecv(sockserver *server, int sockid, char *buf, int len);
