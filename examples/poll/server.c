#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server.h"

// Example server code. Polls for connection and sends a greeting to the new client.

#define check_error(a, b) \
    if (a == b)           \
        goto return_err;

#define LOG(msg) printf("server: %s\n", msg);

int main(void)
{
    // Create server object
    sockserver *server = serverinit(IPv4, TCP, NULL, "3000");
    check_error(server, NULL);

    while (1)
    {
        // Wait for a client to connect
        LOG("waiting for connection...");
        short result = serverpoll(server, -1);

        if (result & POLL_ACCEPT)
        {
            int client_id = serveraccept(server);
            check_error(client_id, -1);
            LOG("a new client connected!");

            // Send message to connected client
            LOG("sending greeting");
            char *msg = "Hello new client!";
            int err = serversend(server, client_id, msg, strlen(msg));
            check_error(err, -1);

            // Close connection
            serverclose(server, client_id);
        }
    }

return_err:
    print_sockerr();
    servershutdown(server);
    return 1;
}