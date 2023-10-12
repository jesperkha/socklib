#pragma once

#include "socket.h"

typedef struct sockclient
{
    int sockfd;
} sockclient;

sockclient *clientconnect(int ipv, int socktype, const char *ipaddr, const char *port);
void clientclose(sockclient *client);

int clientsend(sockclient *client, char *data, int len);
int clientrecv(sockclient *client, char *buf, int len);
