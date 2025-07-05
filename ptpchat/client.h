#pragma once
#include "framework.h"

typedef struct client_t {
	SOCKET socket;
	WSAEVENT event;
} Client;