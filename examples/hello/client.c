#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"

// Example client code. Sends hello to server and receives hello back.

#define check_error(a, b) \
    if (a == b)           \
        goto return_err;

#define LOG(msg) printf("client: %s\n", msg);

int main(void)
{
    // Create client object and connect
    LOG("connecting to server...");
    sockclient *client = clientconnect(IPv4, TCP, NULL, "3000");
    check_error(client, NULL);

    LOG("connection found");

    // Send hello message to server
    LOG("sending message");
    char *msg = "Hello server!";
    int err = clientsend(client, msg, strlen(msg));
    check_error(err, -1);
    LOG("message sent");

    // Wait for response
    LOG("waiting for response...");
    char buf[64] = {0};
    int bytes = clientrecv(client, buf, 64);
    check_error(bytes, -1);
    check_error(bytes, -2);

    printf("client: response received: %s\n", buf);

    // Close connection
    clientclose(client);
    return 0;

return_err:
    print_sockerr();
    clientclose(client);
    return 1;
}