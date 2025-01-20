#pragma once

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib


class Server
{
public:
    SOCKET* clientSocket = new SOCKET;
    std::string serverAddress;
    int port = 5001;
    bool connected = false;
    bool END = false;

    Server(std::string server):serverAddress(server){
        std::cout << "server starting\n";
        Initialize();
        connection();
    }

    ~Server() {
        closesocket(*clientSocket);
        delete clientSocket;
        WSACleanup();
    }

    void Initialize();

    void connection();

    void reconnect();

    void Listening(void (*callback)(BYTE*, DWORD));

    void startListening(void (*callback)(BYTE*, DWORD));
    void sendData(std::string packet);
    void sendData(BYTE* data, DWORD dataSize);


};

