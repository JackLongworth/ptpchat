#include "server.h"

Server::Server(Chatbox* chatbox) {
    this->chatbox = chatbox;
    this->listener = INVALID_SOCKET;
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
    

    PostMessageA(this->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Listening for connections...")));

    WSAEVENT listenEvent = WSACreateEvent();
    WSAEventSelect(listener, listenEvent, FD_ACCEPT);
    this->clients.addClient({ listener, listenEvent });
    this->listener = listener;


    HANDLE hThread = CreateThread(
        NULL,               // default security attributes
        0,                  // default stack size
        Server::HandleIncomingClients,   // thread function
        reinterpret_cast<LPVOID>(this),              // parameter to thread function
        0,                  // default creation flags (run immediately)
        NULL                // optional thread ID
    );
}

DWORD WINAPI Server::HandleIncomingClients(LPVOID lpParam) {

    Server* pThis = reinterpret_cast<Server*>(lpParam);


    while (true) {
        std::vector<WSAEVENT> events;

        for (int i = 0; i < pThis->clients.size(); i++) {
            events.push_back(pThis->clients.getClient(i).event);
        }

        DWORD waitResult = WSAWaitForMultipleEvents(
            events.size(),
            events.data(),
            FALSE,
            WSA_INFINITE,
            FALSE
        );
        

        int index = waitResult - WSA_WAIT_EVENT_0;
        if (index < 0 || index >= pThis->clients.size()) {
            OutputDebugStringA("WSAWaitForMultipleEvents Failed\n");
            break;
        }

        SOCKET currentSock = pThis->clients.getClient(index).socket;
        WSAEVENT currentEvent = pThis->clients.getClient(index).event;

        WSANETWORKEVENTS networkEvents;
        WSAEnumNetworkEvents(currentSock, currentEvent, &networkEvents);

        if (currentSock == pThis->listener && (networkEvents.lNetworkEvents & FD_ACCEPT)) {
            SOCKET clientSock = accept(pThis->listener, NULL, NULL);
            if (clientSock != INVALID_SOCKET) {
                std::string* newClientConnected = new std::string("New Client connected...\n");
                PostMessageA(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(newClientConnected));

                WSAEVENT clientEvent = WSACreateEvent();
                WSAEventSelect(clientSock, clientEvent, FD_READ | FD_CLOSE);
                pThis->clients.addClient({ clientSock, clientEvent });
            }
        }
        else {
            if (networkEvents.lNetworkEvents & FD_READ) {
                if (networkEvents.lNetworkEvents & FD_READ) {
                    char buffer[1024] = { 0 };
                    int bytes = recv(currentSock, buffer, sizeof(buffer), 0);
                    if (bytes > 0) {
                        std::string received = std::string(buffer);
                        std::string debug = "Received : " + received;

                        PostMessageA(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string(debug)));
                    }
                    else {
                        PostMessageA(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Client disconnected.\n")));
                    }
                }
            }

            if (networkEvents.lNetworkEvents & FD_CLOSE) {
                PostMessageA(pThis->chatbox->handle, WM_APPEND_TEXT, 0, reinterpret_cast<LPARAM>(new std::string("Client disconnected.\n")));
                closesocket(currentSock);
                WSACloseEvent(currentEvent);

                pThis->clients.removeClientByIndex(index);
            }
        }

        WSAResetEvent(currentEvent);

    }

    for (int i = 0; i < pThis->clients.size(); i++) {
        closesocket(pThis->clients.getClient(i).socket);
        WSACloseEvent(pThis->clients.getClient(i).event);
    }

    WSACleanup();

    return 0;
}
