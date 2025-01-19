#include "functions.h"
#include <string> 
#include <fstream> 
#include <iostream>
#include <string>
#include <sstream> 

std::wstring systemIP = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(getIP());
WintunManager wintunManager(L"AmirVPN", systemIP, L"255.0.0.0");
Server server(readingIp());
Lobby lobby(&server, getIP());



void callbackLitentToInterface(BYTE* packet, DWORD size)
{
    if (lobby.join) {

        Packet temp = Packet(packet, false, size);
        server.sendData(packet, size);

    }

}
void callbackLiteningToServer(BYTE* packet, DWORD size)
{
    for (int i = 0; i < size; i++) {

        std::cout << packet[i];
    }
    std::cout << "\n";
    //wintunManager.sendPacket(packet, size);
}

std::string readingIp()
{
    // Open the file
    std::ifstream file("server.txt");
    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
    }
    // Read the entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_content = buffer.str();
    // Close the file
    file.close();
    return file_content;
}

std::string GetVolumeSerialNumber()
{
    char volumePath[MAX_PATH] = "C:\\"; // Use the system drive
    DWORD serialNumber = 0;

    // Retrieve the volume serial number
    if (GetVolumeInformation(L"C:\\", nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0)) {
        char buffer[20];
        sprintf_s(buffer, "%08X", serialNumber); // Format as an 8-character hexadecimal string
        return std::string(buffer);
    }
    else {
        std::cerr << "Failed to retrieve volume serial number: " << GetLastError() << std::endl;
        return "";
    }
}

std::string getIP()
{
    std::string serial = GetVolumeSerialNumber();
    std::string ip = "10";
    for (int i = 0; i < 3; i++) {
        ip += ".";
        auto a = serial[i*2] + serial[i*2 + 1];
        if (a > 250)a = a % 250;
        ip += std::to_string(a);
    }
    return ip;
}


;
