#pragma once
#include <thread>
#include <iostream>
#include <winsock2.h> 
#include <windows.h>
#include "wintun.h"
#include <iomanip>
#include <winsock2.h>  // For inet_ntoa
#include <iphlpapi.h>
#include "WintunManager.h"
#include "Packet.h"

#include <netlistmgr.h>
#include <iostream>
#include <atlbase.h> // For CComPtr

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

static WINTUN_CREATE_ADAPTER_FUNC* WintunCreateAdapter;
static WINTUN_CLOSE_ADAPTER_FUNC* WintunCloseAdapter;
static WINTUN_OPEN_ADAPTER_FUNC* WintunOpenAdapter;
static WINTUN_GET_ADAPTER_LUID_FUNC* WintunGetAdapterLUID;
static WINTUN_GET_RUNNING_DRIVER_VERSION_FUNC* WintunGetRunningDriverVersion;
static WINTUN_DELETE_DRIVER_FUNC* WintunDeleteDriver;
static WINTUN_SET_LOGGER_FUNC* WintunSetLogger;
static WINTUN_START_SESSION_FUNC* WintunStartSession;
static WINTUN_END_SESSION_FUNC* WintunEndSession;
static WINTUN_GET_READ_WAIT_EVENT_FUNC* WintunGetReadWaitEvent;
static WINTUN_RECEIVE_PACKET_FUNC* WintunReceivePacket;
static WINTUN_RELEASE_RECEIVE_PACKET_FUNC* WintunReleaseReceivePacket;
static WINTUN_ALLOCATE_SEND_PACKET_FUNC* WintunAllocateSendPacket;
static WINTUN_SEND_PACKET_FUNC* WintunSendPacket;

class WintunManager {
private:


public:

    WintunManager(std::wstring name, std::wstring ip, std::wstring subnet):name(name), IP(ip),subnet(subnet){
        if (!Initialize()) {
            throw std::runtime_error("Error ...");
        }

        adapter = CreateAdapter(name, L"@AmirKamalian24");
        index = GetAdapterIndex();
        Session = StartSession(adapter);
        configurationIP();

    }

    ~WintunManager(){
        // Delete the adapter
        DeleteAdapter(adapter);
        std::cout << "Adapter deleted successfully." << std::endl;
    }


    std::wstring name;
    std::wstring IP;
    std::wstring subnet;
    DWORD adaptor_index;
    WINTUN_ADAPTER_HANDLE adapter;
    WINTUN_SESSION_HANDLE Session;
    HMODULE Wintun;
    DWORD index;



    bool Initialize();

    void sendPacket(BYTE* pkt, DWORD size);
    WINTUN_ADAPTER_HANDLE CreateAdapter(const std::wstring& adapterName, const std::wstring& adapterType);
    WINTUN_SESSION_HANDLE StartSession(WINTUN_ADAPTER_HANDLE adapter);
    void DeleteAdapter(WINTUN_ADAPTER_HANDLE adapter);
    void configurationIP();
    DWORD GetAdapterIndex();
    void startListening(void (*callback)(BYTE*, DWORD));
    void Listening(void (*callback)(BYTE*, DWORD));

};