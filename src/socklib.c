#define __SOCKET__
#include "socket.h"

#define DEBUG 0

static char sockerr[128] = {0};

// Print socket error message
void print_sockerr()
{
    if (DEBUG)
        perror("socket: ");
    fprintf(stderr, "socket: %s\n", sockerr);
}

// Sets sockerr message
void set_sockerr(const char *msg)
{
    strcpy(sockerr, msg);
}

// Windows setup function. Must be called before using sockets on windows.
void windows_init()
{
#ifdef WINDOWS
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed\n");
        exit(1);
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        fprintf(stderr, "winsock2 not available\n");
        WSACleanup();
        exit(2);
    }
#endif
}

// Returns pointer to response address info. NULL on fail.
struct addrinfo *get_options(int ipv, int socktype, const char *ipaddr, const char *port, bool passive)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = ipv;
    hints.ai_socktype = socktype;

    if (passive && ipaddr == NULL)
        hints.ai_flags = AI_PASSIVE;

    int status;
    struct addrinfo *res = malloc(sizeof hints);
    if ((status = getaddrinfo(ipaddr, port, &hints, &res)) != 0)
    {
        strcpy(sockerr, gai_strerror(status));
        return NULL;
    }

    return res;
}

// Returns socket file descriptor. options is from get_address_info(). Returns -1 on error.
int bind_socket(struct addrinfo *options)
{
    int sockfd;
    struct addrinfo *p;
    for (p = options; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        // Reuse port if not closed properly
        DEF_YES;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, yes, sizeof(int)) == -1)
        {
            set_sockerr("bind_socket(): setsockopt fail");
            return -1;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            continue;

        // Socket bound
        break;
    }

    if (p == NULL)
    {
        set_sockerr("bind_socket(): failed to bind");
        return -1;
    }

    return sockfd;
}

// Returns socket file descriptor. options is from get_address_info(). Returns -1 on error.
int connect_socket(struct addrinfo *options)
{
    int sockfd;
    struct addrinfo *p;
    for (p = options; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
            continue;

        // Connection made
        break;
    }

    if (p == NULL)
    {
        set_sockerr("connect_socket(): failed to connect");
        return -1;
    }

    return sockfd;
}