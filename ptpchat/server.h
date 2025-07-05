#pragma once


#include "framework.h"
#include "chatbox.h"
#include "client_list.h"
#include "client.h"
#include <vector>
#include <map>

#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 4


class Server
{

public:
	Server(Chatbox* chatbox);
	int Start();

private:
	static DWORD WINAPI HandleIncomingClients(LPVOID lpParam);

public:
	ClientList clients;

private:
	SOCKET listener;

	Chatbox* chatbox;
	
};

