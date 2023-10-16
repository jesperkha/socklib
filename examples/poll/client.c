#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"

// Example client code. Connects to server and prints greeting.

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

    // Wait for greeting
    LOG("waiting for greeting...");
    char buf[64] = {0};
    int bytes = clientrecv(client, buf, 64);
    check_error(bytes, -1);
    check_error(bytes, -2);

    printf("client: greeting received: %s\n", buf);

    // Close connection
    clientclose(client);
    return 0;

return_err:
    print_sockerr();
    clientclose(client);
    return 1;
}