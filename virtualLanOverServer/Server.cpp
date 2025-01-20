#include "Server.h"

void Server::Initialize()
{
    // Initialize Winsock
    std::cout << "server starting\n";
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    // Create a socket
    *clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }
    return;
}

void Server::connection()
{
    std::cout << "start connecting\n";
    // Set up server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

    // Connect to the server
    if (connect(*clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(*clientSocket);
        WSACleanup();
        return;
    }
    std::cout << "Connected to server at " << serverAddress << ":" << port << std::endl;
    connected = true;
}

void Server::reconnect() {
    // Close the old socket if it's still open
    if (*clientSocket != INVALID_SOCKET) {
        closesocket(*clientSocket);
        *clientSocket = INVALID_SOCKET;
    }

    // Create a new socket
    *clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr);

    // Reconnect to the server
    if (connect(*clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Reconnection failed: " << WSAGetLastError() << std::endl;
        closesocket(*clientSocket);
        *clientSocket = INVALID_SOCKET;
        return;
    }

    std::cout << "Reconnected to the server!" << std::endl;
    connected = true;
}

void Server::Listening(void (*callback)(BYTE*, DWORD))
{
    // Send and receive data
    char buffer[1024];
    std::string message;

    while (!END) {

        if (!connected)continue;

        // Receive response from server
        int bytesReceived = recv(*clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {

            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            //std::cout << "Server response: " << buffer << std::endl;
            callback((BYTE*)buffer, bytesReceived);
        }
        else if (bytesReceived == 0) {
            std::cout << "Server closed the connection." << std::endl;
            connected = false;
            
        }
        else {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            connected = false;
        }

    }
}

void Server::startListening(void(*callback)(BYTE*, DWORD))
{
    std::thread listen(&Server::Listening, this, callback);
    listen.detach(); // Detach the thread if you don't want to join it later
}

void Server::sendData(std::string packet)
{
    if (connected) {
        // Send message to server
        if (send(*clientSocket, packet.c_str(), packet.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            connected = false;
        }
    }
}
void Server::sendData(BYTE* data, DWORD dataSize)
{
    if (connected) {
        // Send binary data to the client
        if (send(*clientSocket, reinterpret_cast<const char*>(data), dataSize, 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
    }
}



