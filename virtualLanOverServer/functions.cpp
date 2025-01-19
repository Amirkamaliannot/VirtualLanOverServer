#include "functions.h"
#include <string> 

//WintunManager wintunManager(L"AmirVPN", L"10.10.10.10", L"255.0.0.0");

void callbackLitentToInterface(BYTE* packet, DWORD size)
{
    Packet temp = Packet(packet, false, size);


}
void callbackLiteningToSerser(BYTE* packet, DWORD size)
{
    for (int i = 0; i < size; i++) {

        std::cout << packet[i];
    }
    std::cout << "\n";
    //wintunManager.sendPacket(packet, size);
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
