#pragma once

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib


class Server
{
public:
    SOCKET clientSocket;
    std::string serverAddress = "127.0.0.1";
    int port = 5001;

    bool END = false;

    Server() {
        std::cout << "server starting\n";
        Initialize();
        connection();
    }

    ~Server() {
        // Clean up
        closesocket(clientSocket);
        WSACleanup();
    }

    void Initialize();

    void connection();

    void Listening(void (*callback)(BYTE*, DWORD));

    void startListening(void (*callback)(BYTE*, DWORD));
    void sendData(std::string packet);
    void sendData(BYTE* data, DWORD dataSize);


};

