#pragma once
#include <steam/steam_api.h>
#include <steam/isteamnetworkingmessages.h>
#include <steam/steamtypes.h>
#include <vector>
#include <string>
#include <functional>
#include <windows.h>
#include <iostream>
#define CHANNEL_ID 0 // Channel ID for sending/receiving messages
#define MAX_MESSAGES 10 // Maximum messages to retrieve at once


class DataTransfer {

public:
    ISteamNetworkingMessages* m_networkingMessages;

    // Constructor
    DataTransfer() :
        m_steamNetworkingMessagesSessionFailed(this, &DataTransfer::onSessionFailed),
        m_steamNetworkingMessagesSessionRequest(this, &DataTransfer::onSessionRequest)
    {
    
        m_networkingMessages = SteamNetworkingMessages();
        if (!m_networkingMessages) {
            throw std::runtime_error("Failed to initialize Steam Networking Messages");
        }
    
    }

    void  ProcessIncomingMessages(void(*callback)(BYTE*, DWORD));
    EResult SendMessageW(CSteamID userID, const void* data, uint32 dataSize, int flags = k_nSteamNetworkingSend_Reliable);
    void onSessionFailed(SteamNetworkingMessagesSessionFailed_t* callback);
    void onSessionRequest(SteamNetworkingMessagesSessionRequest_t* callback);
    CCallback<DataTransfer, SteamNetworkingMessagesSessionFailed_t> m_steamNetworkingMessagesSessionFailed;
    CCallback<DataTransfer, SteamNetworkingMessagesSessionRequest_t> m_steamNetworkingMessagesSessionRequest;

private:

};