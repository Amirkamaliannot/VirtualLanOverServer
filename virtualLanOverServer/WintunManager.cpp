#include "WintunManager.h"
#include <ws2tcpip.h>

bool WintunManager::Initialize()
{

    Wintun = LoadLibraryExW(L"wintun.dll", NULL, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!Wintun) return NULL;
#define X(Name) ((*(FARPROC *)&Name = GetProcAddress(Wintun, #Name)) == NULL)
    if (X(WintunCreateAdapter) || X(WintunCloseAdapter) || X(WintunOpenAdapter) || X(WintunGetAdapterLUID) ||
        X(WintunGetRunningDriverVersion) || X(WintunDeleteDriver) || X(WintunSetLogger) || X(WintunStartSession) ||
        X(WintunEndSession) || X(WintunGetReadWaitEvent) || X(WintunReceivePacket) || X(WintunReleaseReceivePacket) ||
        X(WintunAllocateSendPacket) || X(WintunSendPacket))
#undef X
    {
        DWORD LastError = GetLastError();
        FreeLibrary(Wintun);
        SetLastError(LastError);
        return NULL;
    }
    return Wintun;
}



void WintunManager::sendPacket(BYTE* rawPacketData, DWORD packetSize)
{
    BYTE* packet = static_cast<BYTE*>(WintunAllocateSendPacket(Session, packetSize));
    if (!packet) {
        std::cerr << "Failed to allocate packet buffer." << std::endl;
        return;
    }

    // Fill the packet
    memcpy(packet, rawPacketData, packetSize);

    WintunSendPacket(Session, packet);

}

WINTUN_ADAPTER_HANDLE WintunManager::CreateAdapter(const std::wstring& adapterName, const std::wstring& adapterType)
{
    if (!WintunCreateAdapter) {
        std::cerr << "WintunCreateAdapter not loaded" << std::endl;
        return nullptr;
    }
    std::cout << "Adapter created successfully." << std::endl;
    return WintunCreateAdapter(adapterName.c_str(), adapterType.c_str(), nullptr);
}



WINTUN_SESSION_HANDLE WintunManager::StartSession(WINTUN_ADAPTER_HANDLE adapter)
{
    if (!WintunCreateAdapter) {
        std::cerr << "WintunCreateAdapter not loaded" << std::endl;
        return nullptr;
    }
    return WintunStartSession(adapter, 0x400000);
}



void WintunManager::DeleteAdapter(WINTUN_ADAPTER_HANDLE adapter)
{
    if (WintunCloseAdapter && adapter) {
        WintunCloseAdapter(adapter);
    }
}



DWORD WintunManager::GetAdapterIndex()
{
    ULONG outBufLen = 0;
    DWORD dwRetVal = 0;
    // Initial call to get the buffer size needed
    GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &outBufLen);
    PIP_ADAPTER_ADDRESSES adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(outBufLen);

    if ((dwRetVal = GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapterAddresses, &outBufLen)) == NO_ERROR) {
        PIP_ADAPTER_ADDRESSES currentAdapter = adapterAddresses;

        while (currentAdapter) {
            // Compare adapter name
            if (this->name == currentAdapter->FriendlyName) {
                DWORD index = currentAdapter->IfIndex;
                free(adapterAddresses);
                return index;
            }
            currentAdapter = currentAdapter->Next;
        }
    }
    else {
        std::cerr << "GetAdaptersAddresses failed. Error: " << dwRetVal << std::endl;
    }
    free(adapterAddresses);
    return 0; 
}



void WintunManager::startListening(void (*callback)(BYTE*, DWORD))
{
    std::thread listen(&WintunManager::Listening, this, callback);
    listen.detach(); // Detach the thread if you don't want to join it later
}




void WintunManager::Listening( void(*callback)(BYTE*, DWORD)  )
{
    for (;;)
    {
        DWORD IncomingPacketSize;
        BYTE* IncomingPacket = WintunReceivePacket(Session, &IncomingPacketSize);
        if (IncomingPacket)
        {
            callback(IncomingPacket, IncomingPacketSize);
            WintunReleaseReceivePacket(Session, IncomingPacket);
        }
        else if (GetLastError() == ERROR_NO_MORE_ITEMS)
            WaitForSingleObject(WintunGetReadWaitEvent(Session), INFINITE);
        else
        {
            std::cout << "Packet read failed\n";
            break;
        }
    }
}

void WintunManager::configurationIP()
{
    IN_ADDR ipAddr;
    IN_ADDR subnetMask;
    InetPton(AF_INET, IP.c_str(), &ipAddr);
    InetPton(AF_INET, subnet.c_str(), &subnetMask);
    IPAddr  ipBinary = ipAddr.S_un.S_addr;
    IPAddr  maskBinary = subnetMask.S_un.S_addr;
    // Define variables
    DWORD interfaceIndex = index;
    ULONG nteContext = 0;
    ULONG nteInstance = 0;
    // Add IP address
    DWORD result = AddIPAddress(ipBinary, maskBinary, interfaceIndex, &nteContext, &nteInstance);
    if (result == NO_ERROR) {
        std::cout << "IP address added successfully!" << std::endl;
    }
    else {
        std::cerr << "Failed to add IP address. Error code: " << result << std::endl;
    }

}