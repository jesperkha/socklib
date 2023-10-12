#define __SOCKET__
#include "server.h"

static int wininit = 0; // Check if windows_init was called

// Returns new socket server instance using given ip version, socket type, ip address, and port number.
// If ipaddr is NULL, localhost is used. If port is NULL, the server assigns the first available port.
// Returns NULL on error.
sockserver *serverinit(int ipv, int socktype, const char *ipaddr, const char *port)
{
    // Call WSAStartup. Macro removes call on Linux
    if (wininit == 0)
    {
        windows_init();
        wininit = 1;
    }

    sockserver *server = calloc(1, sizeof(sockserver));
    server->sockets = malloc(SERVER_MINSOCKSLEN);
    server->socket_type = socktype;
    server->ip_version = ipv;

    struct addrinfo *options = get_options(ipv, socktype, ipaddr, port, true);
    if (options == NULL)
        goto return_err;

    if ((server->sockfd = bind_socket(options)) == -1)
        goto return_err;

    if (listen(server->sockfd, SERVER_BACKLOG) == -1)
        goto return_err;

    freeaddrinfo(options);
    return server;

return_err:
    print_sockerr(); // Print previous socket error
    set_sockerr("failed to set up server");
    return NULL;
}

// Sends shutdown signal to connected clients and frees server object.
void servershutdown(sockserver *server)
{
    // Todo: server shutdown signal

    if (server != NULL)
    {
        close(server->sockfd);
        free(server->sockets);
        free(server);
    }
}

// Blocks until connection comes through. Returns id of accepted connection, -1 on error.
// The id is not the actual socket, but the index in the servers socket array.
int serveraccept(sockserver *server)
{
    int clientfd = accept(server->sockfd, NULL, NULL);
    if (clientfd == -1)
    {
        set_sockerr("server failed to accept connection");
        return -1;
    }

    server->sockets[server->num_conn] = clientfd;
    if (server->num_conn == server->socket_cap)
    {
        // Realloc not expected to fail
        int newsize = server->socket_cap + SERVER_MINSOCKSLEN;
        server->sockets = realloc(server->sockets, newsize);
    }

    return server->num_conn++;
}

// Closes the given socket. sockid may be reused.
void serverclosesocket(sockserver *server, int sockid)
{
    // Todo: server close socket
}

// Send data over given socket id. Returns -1 on error. If a message couldn't be sent
// in one go, it retries until the whole message is sent. serversend() does not cover
// cases where send() fails when retrying.
int serversend(sockserver *server, int sockid, const char *data, int len)
{
    ssize_t sent = send(server->sockets[sockid], data, len, 0);
    if (sent == -1)
        goto return_err;

    // If the whole message couldnt be sent, retry until done
    if (sent < len)
    {
        int total = sent;
        while (total < len)
        {
            sent = send(server->sockets[sockid], data + total, len - total, 0);
            if (sent == -1)
                goto return_err;

            total += sent;
        }
    }

    return 0;

return_err:
    set_sockerr("server failed to send");
    return -1;
}

// Blocks until server recieves data on given socket id. Writes data to buf.
// Returns number bytes recieved, -1 on error. If the client disconnects, the
// socket is closed and the function returns -2.
int serverrecv(sockserver *server, int sockid, char *buf, int len)
{
    int bytes = recv(server->sockets[sockid], buf, len, 0);
    if (bytes == -1)
    {
        set_sockerr("server failed to receive");
        return -1;
    }

    // Client has disconnected, close socket and free sockid
    if (bytes == 0)
    {
        set_sockerr("client disconnected");
        serverclosesocket(server, sockid);
        return -2;
    }

    return bytes;
}