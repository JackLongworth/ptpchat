#pragma once

#include "framework.h"

#include "chatbox.h"
#include "server.h"

class Network
{
public:
	static int StartupServer(Chatbox* chatbox);
	static int BroadcastMessage(std::string* message);

private:
	static int InitialiseWinsock();

public:

private:
	static WSADATA wsaData;
	static Server* server;
};

