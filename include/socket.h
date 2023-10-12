#pragma once

// Public

#define IPv4 2  // AF_INET
#define IPv6 23 // AF_INET6
#define IPany 0 // AF_UNSPEC
#define TCP 1   // SOCK_STREAM
#define UDP 2   // SOCK_DGRAM

void print_sockerr();

// Private

#ifdef __SOCKET__
#include "sockdef.h"

void windows_init();
int bind_socket(struct addrinfo *options);
int connect_socket(struct addrinfo *options);
struct addrinfo *get_options(int ip_v, int socket_type, const char *ipaddr, const char *port, bool passive);

void set_sockerr(const char *msg);

#endif
