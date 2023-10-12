#pragma once

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// Windows/linux specific includes

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <winsock2.h>
#include <ws2tcpip.h>

#define close closesocket
#define DEF_YES const char *yes = "1";
#define WINDOWS

#else

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEF_YES int yes = 1;
#define LINUX

#endif

/*

Notes for windows:

- setsockopt uses the string 1 not int
- need to init WSAsetup
- WSAGetLastError to get error code
- do not pass last two args to accept(), it will raise an error, use NULL
- inet_ntop() is not a thing, use inet_ntoa()
- close() is closesocket()

*/