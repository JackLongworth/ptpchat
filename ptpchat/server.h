#pragma once


#include "framework.h"
#include "chatbox.h"
#include "client_list.h"
#include "client.h"
#include <string.h>
#include <vector>
#include <map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 4

typedef struct Host {
	SOCKET socket;
	WSAEVENT event;
} Host;

class Server
{

public:
	Server(Chatbox* chatbox);
	int Start();

private:
	static int getRemoteIPFromSocket(SOCKET socket, char* ipStr, int ipStrLen);
	static DWORD WINAPI HandleNewConnections(LPVOID lpParam);
	static DWORD WINAPI HandleIncomingMessages(LPVOID lpParam);

public:
	ClientList clients;
private:
	Host host;
	Chatbox* chatbox;
};

