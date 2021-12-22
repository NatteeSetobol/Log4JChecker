#ifndef _NIX_SOCKET_H_
#define _NIX_SOCKET_H_
#include <string.h>
#include <stdio.h>
#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "stringz.h"

struct platform_socket
{
	int socket;
	bool connected;
};


int CloseSocket(struct platform_socket* platformSocket);
//struct platform_socket CreateSocket(memory_arena *memoryArena,char* hostname, int port);
#endif
