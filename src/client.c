#define __SOCKET__
#include "client.h"

static int wininit = 0; // Check if windows_init was called

// Returns new socket client instance connected to given ip and port using given ip
// version and socket type. Return NULL on error.
sockclient *clientconnect(int ipv, int socktype, const char *ipaddr, const char *port)
{
    // Call WSAStartup. Macro removes call on Linux
    if (wininit == 0)
    {
        windows_init();
        wininit = 1;
    }

    sockclient *client = calloc(1, sizeof(sockclient));

    struct addrinfo *options = get_options(ipv, socktype, ipaddr, port, false);
    if (options == NULL)
        goto return_err;

    if ((client->sockfd = connect_socket(options)) == -1)
        goto return_err;

    freeaddrinfo(options);
    return client;

return_err:
    print_sockerr();
    set_sockerr("client failed to connect");
    return NULL;
}

// Closes client socket connection and frees client object
void clientclose(sockclient *client)
{
    if (client != NULL)
    {
        close(client->sockfd);
        free(client);
    }
}

// Send data with length len. Returns -1 on error. If a message couldn't be sent in one go
// it retries until the whole message is sent. clientsend() does not cover cases where send()
// fails when retrying.
int clientsend(sockclient *client, char *data, int len)
{
    ssize_t sent = send(client->sockfd, data, len, 0);
    if (sent == -1)
        goto return_err;

    // If the whole message couldnt be sent, retry until done
    if (sent < len)
    {
        int total = sent;
        while (total < len)
        {
            sent = send(client->sockfd, data + total, len - total, 0);
            if (sent == -1)
                goto return_err;

            total += sent;
        }
    }

    return 0;

return_err:
    set_sockerr("client failed to send");
    return -1;
}

// Blocks until data is received, written to buf with max length len.
// Returns -1 on error. If connection was closed, client is closed and
// freed and function returns -2.
int clientrecv(sockclient *client, char *buf, int len)
{
    int bytes = recv(client->sockfd, buf, len, 0);
    if (bytes == -1)
    {
        set_sockerr("client failed to receive");
        return -1;
    }

    if (bytes == 0)
    {
        set_sockerr("client got disconnected from server");
        clientclose(client);
        return -2;
    }

    return bytes;
}