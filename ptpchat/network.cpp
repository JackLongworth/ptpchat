#include "network.h"

WSADATA Network::wsaData = { 0 };

Server* Network::server = nullptr;

int Network::InitialiseWinsock() {
	int result;

	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		OutputDebugStringA("WSAStartup Failed\n");
		return 1;
	}

	return 0;
}

int Network::StartupServer(Chatbox* chatbox) {
	if (Network::InitialiseWinsock() != 0) {
		OutputDebugStringA("Couldn't initialise winsock\n");
		return 1;
	}
	OutputDebugStringA("Server starting up\n");

	Network::server = new Server(chatbox);
	Network::server->Start();

	return 0;
}

int Network::BroadcastMessage(std::string* message) {
	if (server == nullptr) return 1;

	std::vector<Client> clients = Network::server->clients.snapshot();

	for (Client client : clients) {
		send(client.socket, message->c_str(), static_cast<int>(message->size()), 0);
	}
	
	return 0;
}