#include "server.h"

Server::Server(Chatbox* chatbox) {
    this->chatbox = chatbox;
    this->host.socket = INVALID_SOCKET;
}

int Server::Start() {

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        OutputDebugStringA("getaddrinfo failed: " + iResult);
        WSACleanup();
        return 1;
    }

    
    SOCKET listener = INVALID_SOCKET;

    // Create a SOCKET for the server to listen for client connections
    listener = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (listener == INVALID_SOCKET) {
        OutputDebugStringA("Error at socket(): " + WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(listener, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    if (iResult == SOCKET_ERROR) {
        OutputDebugStringA("bind failed with error : " + WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR) {
        OutputDebugStringA("Listen failed with error: " + WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    
    PostMessage(this->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Listening for connections...")));

    WSAEVENT listenEvent = WSACreateEvent();
    WSAEventSelect(listener, listenEvent, FD_ACCEPT);
    this->host = { listener, listenEvent };


    HANDLE newConnectionsThread = CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        Server::HandleNewConnections,   // thread function
        reinterpret_cast<LPVOID>(this),              // parameter to thread function
        0,                  // default creation flags (run immediately)
        NULL                // optional thread ID
    );

    return 0;
}

int Server::getRemoteIPFromSocket(SOCKET socket, char *ipStr, int ipStrLen) {

    int success = 0;

    sockaddr_in addr;
    int addrSize = sizeof(addr);

    if (getsockname(socket, (sockaddr*)&addr, &addrSize) == 0) {
        inet_ntop(AF_INET, &(addr.sin_addr), ipStr, ipStrLen);
    }
    else {
        success = 1;
    }

    return success;
}

DWORD WINAPI Server::HandleIncomingMessages(LPVOID lpParam) {

    Server* pThis = reinterpret_cast<Server*>(lpParam);

    while (true) {
        std::vector<WSAEVENT> events;

        for (int i = 0; i < pThis->clients.size(); i++) {
            events.push_back(pThis->clients.getClient(i).event);
        }

        // Note that WSAWaitForMultipleEvents will return the smallest index if there are two events signalled simultaneously
        DWORD waitResult = WSAWaitForMultipleEvents(
            static_cast<DWORD>(events.size()),
            events.data(),
            FALSE,
            WSA_INFINITE,
            FALSE
        );


        // calculate the index of the signalled event
        int index = waitResult - WSA_WAIT_EVENT_0;
        if (index < 0 || index >= pThis->clients.size()) {
            OutputDebugStringA("WSAWaitForMultipleEvents Failed\n");
            break;
        }

        SOCKET currentSock = pThis->clients.getClient(index).socket;
        WSAEVENT currentEvent = pThis->clients.getClient(index).event;

        WSANETWORKEVENTS networkEvents;
        WSAEnumNetworkEvents(currentSock, currentEvent, &networkEvents);

        if (networkEvents.lNetworkEvents & FD_READ) {
            char buffer[1024] = { 0 };
            int bytes = recv(currentSock, buffer, sizeof(buffer), 0);
            if (bytes > 0) {
                std::string received = std::string(buffer);
                std::string debug = "Received : " + received;

                PostMessage(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string(debug)));
            }
            else {
                PostMessage(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Client disconnected.\n")));
            }
        }

        if (networkEvents.lNetworkEvents & FD_CLOSE) {
            PostMessage(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Client disconnected.\n")));
            closesocket(currentSock);
            WSACloseEvent(currentEvent);

            pThis->clients.removeClientByIndex(index);

            if (pThis->clients.size() == 0) {
                break;
            }
        }

        WSAResetEvent(currentEvent);
    }

    for (int i = 0; i < pThis->clients.size(); i++) {
        closesocket(pThis->clients.getClient(i).socket);
        WSACloseEvent(pThis->clients.getClient(i).event);
    }

    return 0;
}

DWORD WINAPI Server::HandleNewConnections(LPVOID lpParam) {

    Server* pThis = reinterpret_cast<Server*>(lpParam);

    HANDLE incomingMessagesThread;

    while (true) {
       
        DWORD result = WaitForSingleObject(pThis->host.event, INFINITE);

        if (result != WAIT_OBJECT_0) {
            return 1;
        }

        WSANETWORKEVENTS networkEvents;
        WSAEnumNetworkEvents(pThis->host.socket, pThis->host.event, &networkEvents);

        if (networkEvents.lNetworkEvents & FD_ACCEPT) {
            SOCKET clientSock = accept(pThis->host.socket, NULL, NULL);
            if (clientSock != INVALID_SOCKET) {
                size_t currentConnectedClients = pThis->clients.size();
                
                std::string* newClientConnected = new std::string("New Client connected...\n");
                PostMessage(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(newClientConnected));

                //TODO: Need to send a 'welcome' message to a new connection client 
                //The message should be a json of a format something like this: 
                // { connected_clients: [{ username: ..., ip_addr: ... }] }
                // This can be used to initialise their chat session (potentially could add a chat history or something)
                std::vector<char*> connectedClientIPs;
                for (Client client : pThis->clients.snapshot()) {
                    connectedClientIPs.push_back(client.ip);
                }

                json j;
                j["connected_clients"] = connectedClientIPs;
                std::string welcomeMessageJson = j.dump();
                send(clientSock, welcomeMessageJson.c_str(), static_cast<int>(welcomeMessageJson.size()), 0);

                char ipStr[INET_ADDRSTRLEN];
                if (Server::getRemoteIPFromSocket(clientSock, ipStr, INET_ADDRSTRLEN) == 0) {
                    WSAEVENT clientEvent = WSACreateEvent();
                    WSAEventSelect(clientSock, clientEvent, FD_READ | FD_CLOSE);

                    Client connectedClient;
                    connectedClient.socket = clientSock;
                    connectedClient.event = clientEvent;
                    strncpy_s(connectedClient.ip, INET_ADDRSTRLEN, ipStr, _TRUNCATE);

                    pThis->clients.addClient(connectedClient);
                }

                if (currentConnectedClients == 0) {
                    incomingMessagesThread = CreateThread(
                        NULL,
                        0,
                        Server::HandleIncomingMessages,
                        reinterpret_cast<LPVOID>(pThis),
                        0,
                        NULL
                    );
                }
            }
        }

        WSAResetEvent(pThis->host.event);

    }

    closesocket(pThis->host.socket);
    WSACloseEvent(pThis->host.event);

    WSACleanup();

    return 0;
}
