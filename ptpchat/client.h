#pragma once
#include "framework.h"

typedef struct client_t {
	SOCKET socket;
	WSAEVENT event;
	char ip[INET_ADDRSTRLEN];
	std::string username;
} Client;