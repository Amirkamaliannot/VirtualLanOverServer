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

    void  ProcessIncomingMessages(void(*callback)(BYTE*, DWORD)) {

        // Array to hold pointers to received messages
        ISteamNetworkingMessage* pMessages[MAX_MESSAGES] = { nullptr };

        // Receive messages on the specified channel
        int numMessages = m_networkingMessages->ReceiveMessagesOnChannel(
            CHANNEL_ID,
            pMessages,
            MAX_MESSAGES
        );
        if (numMessages > 0) {
            std::cout << "Received " << numMessages << " messages." << std::endl;

            for (int i = 0; i < numMessages; ++i) {
                ISteamNetworkingMessage* pMessage = pMessages[i];

                // Access the message data
                //std::cout << "Message from SteamID: " << pMessage->m_identityPeer.GetSteamID64() << std::endl;
                //std::cout << "Message size: " << pMessage->m_cbSize << " bytes" << std::endl;
                //std::cout << "Message content: " << std::string((char*)pMessage->m_pData, pMessage->m_cbSize)<< std::endl;
                callback((BYTE*)pMessage->m_pData, pMessage->m_cbSize);
                std::cout << "get 1;\n";
                // Release the message to free its resources
                pMessage->Release();
            }


        }
        else if (numMessages == 0) {
            //std::cout << "No messages available." << std::endl;
        }
        else {
            std::cerr << "Error receiving messages: " << numMessages << std::endl;
        }
    }

    // Send message and track its status
    EResult SendMessageW(CSteamID userID, const void* data, uint32 dataSize, int flags = k_nSteamNetworkingSend_Reliable) {

        SteamNetworkingIdentity remoteIdentity;
        remoteIdentity.SetSteamID(userID);

        // Send the message
        EResult result = m_networkingMessages->SendMessageToUser(
            remoteIdentity,
            data,
            dataSize,
            flags,
            0  // Default channel
        );
        // Initial result is just for message queuing
        if (result == k_EResultOK) {
            // Message was queued successfully
            // Actual send result will come through callback
        }

        std::cout << "sent!\n";
        return result;
    }



    void onSessionFailed(SteamNetworkingMessagesSessionFailed_t* callback) {

        std::cout << "onSessionFailed\n";

    }    

    void onSessionRequest(SteamNetworkingMessagesSessionRequest_t* callback) {
        std::cout << "SessionRequest\n";

        // Validate the remote identity
        SteamNetworkingIdentity identityRemote;
        identityRemote.SetSteamID(callback->m_identityRemote.GetSteamID());

        std::cout << "SteamID: " << identityRemote.GetSteamID().ConvertToUint64() << std::endl;

        // Attempt to accept the session
        bool result = m_networkingMessages->AcceptSessionWithUser(identityRemote);
        if (result) {
            std::cout << "SessionAccepted\n";
        }
        else {
            std::cerr << "SessionNOTAccepted: Check remote user, network, or state.\n";
        }
    }

    // CCallback member for LobbyChatUpdate_t
    CCallback<DataTransfer, SteamNetworkingMessagesSessionFailed_t> m_steamNetworkingMessagesSessionFailed;
    CCallback<DataTransfer, SteamNetworkingMessagesSessionRequest_t> m_steamNetworkingMessagesSessionRequest;

    bool done=false;

private:

};