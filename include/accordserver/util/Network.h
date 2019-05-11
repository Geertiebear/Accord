#ifndef ACCORD_UTIL_NETWORK_H
#define ACCORD_UTIL_NETWORK_H

#ifdef _WIN32
#include <winsock2.h>
#define ACCORD_SOCKADDR SOCKADDR_IN
#define ACCORD_CLOSESOCKET(s) closesocket(s);
#else
#include <unistd.h>
#include <netinet/in.h>
#define ACCORD_SOCKADDR struct sockaddr_in
#define ACCORD_CLOSESOCKET(s) close(s);
#endif

#endif
