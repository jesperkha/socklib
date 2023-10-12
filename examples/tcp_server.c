#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server.h"

// Example server code. Receives hello from client and sends hello back.

#define check_error(a, b) \
    if (a == b)           \
        goto return_err;

#define LOG(msg) printf("server: %s\n", msg);

int main(void)
{
    // Create server object
    sockserver *server = serverinit(IPv4, TCP, NULL, "3000");
    check_error(server, NULL);

    // Wait for a client to connect
    LOG("waiting for connection...");
    int client_id = serveraccept(server);
    check_error(client_id, -1);

    LOG("connection found");

    // Wait for message
    LOG("waiting for message...");
    char buf[64];
    int bytes = serverrecv(server, client_id, buf, 64);
    check_error(bytes, -1);
    check_error(bytes, -2);

    printf("server: message received: %s\n", buf);

    // Send message to connected client
    LOG("sending response");
    char *msg = "Hello client!";
    int err = serversend(server, client_id, msg, strlen(msg));
    check_error(err, -1);

    LOG("response sent");

    // Shut down server
    servershutdown(server);
    return 0;

return_err:
    print_sockerr();
    servershutdown(server);
    return 1;
}