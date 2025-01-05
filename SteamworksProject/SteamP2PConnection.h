#include <steam/steam_api.h>
#include <steam/isteamnetworkingmessages.h>
#include <steam/steamtypes.h>
#include <vector>
#include <string>
#include <functional>
#include <iostream>
#define CHANNEL_ID 0 // Channel ID for sending/receiving messages
#define MAX_MESSAGES 10 // Maximum messages to retrieve at once

class MySteamApp {
public:
    // Constructor
    MySteamApp() :m_CallbackLobbyChatUpdate(this, &MySteamApp::OnLobbyChatUpdate) {}


    void OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback) {

        done = true;
        CSteamID lobbyID = CSteamID(pCallback->m_ulSteamIDLobby);
        CSteamID userChangedID = CSteamID(pCallback->m_ulSteamIDUserChanged);
        CSteamID makingChangeID = CSteamID(pCallback->m_ulSteamIDMakingChange);
        uint32 changeFlags = pCallback->m_rgfChatMemberStateChange;

        //std::cout << "Lobby ID: " << lobbyID.ConvertToUint64() << std::endl;
        //std::cout << "User Changed: " << userChangedID.ConvertToUint64() << std::endl;

        //// Handle specific changes
        //if (changeFlags & k_EChatMemberStateChangeEntered) {
        //    std::cout << "User joined the lobby.\n";
        //}
        //if (changeFlags & k_EChatMemberStateChangeLeft) {
        //    std::cout << "User left the lobby.\n";
        //}
        //if (changeFlags & k_EChatMemberStateChangeDisconnected) {
        //    std::cout << "User disconnected from the lobby.\n";
        //}
        //if (changeFlags & k_EChatMemberStateChangeKicked) {
        //    std::cout << "User was kicked from the lobby.\n";
        //}
        //if (changeFlags & k_EChatMemberStateChangeBanned) {
        //    std::cout << "User was banned from the lobby.\n";
        //}

    }

    bool done=false;

private:
    // CCallback member for LobbyChatUpdate_t
    CCallback<MySteamApp, LobbyChatUpdate_t> m_CallbackLobbyChatUpdate;
};

class dataTransfer {
public:
    ISteamNetworkingMessages* m_networkingMessages;
    SteamNetworkingIdentity m_remoteIdentity;
    bool m_isConnected;

    // Constructor
    dataTransfer() :
        m_steamNetworkingMessagesSessionFailed(this, &dataTransfer::onSessionFailed),
        m_steamNetworkingMessagesSessionRequest(this, &dataTransfer::onSessionRequest)
    {
    
        m_networkingMessages = SteamNetworkingMessages();
        if (!m_networkingMessages) {
            throw std::runtime_error("Failed to initialize Steam Networking Messages");
        }
    
    }

    //// Send message and track its status
    //EResult SendMessage(const void* data, uint32 dataSize, int flags = k_nSteamNetworkingSend_Reliable) {
    //    if (!m_isConnected) {
    //        return k_EResultFail;
    //    }

    //    // Send the message
    //    EResult result = m_networkingMessages->SendMessageToUser(
    //        m_remoteIdentity,
    //        data,
    //        dataSize,
    //        flags,
    //        0  // Default channel
    //    );
    //    // Initial result is just for message queuing
    //    if (result == k_EResultOK) {
    //        // Message was queued successfully
    //        // Actual send result will come through callback
    //    }
    //    return result;
    //}



    void onSessionFailed(SteamNetworkingMessagesSessionFailed_t* callback) {

        std::cout << "onSessionFailed\n";

    }    
    void onSessionRequest(SteamNetworkingMessagesSessionRequest_t* callback) {

        std::cout << "SessionRequest\n";

        SteamNetworkingIdentity identityRemote;
        identityRemote.SetSteamID(callback->m_identityRemote.GetSteamID());

        // Accept or reject the connection
        bool result = m_networkingMessages->AcceptSessionWithUser(identityRemote);
        if (result) {
            m_isConnected = true;
            m_remoteIdentity = identityRemote;
            std::cout << "AcceptSession\n";
        }
        else {

            std::cout << "AcceptNOTSession\n";

        }

    }

    void ProcessIncomingMessages() {

        done = true;
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
                std::cout << "Message from SteamID: "
                    << pMessage->m_identityPeer.GetSteamID64()
                    << std::endl;

                std::cout << "Message size: " << pMessage->m_cbSize << " bytes" << std::endl;
                std::cout << "Message content: "
                    << std::string((char*)pMessage->m_pData, pMessage->m_cbSize)
                    << std::endl;

                // Release the message to free its resources
                pMessage->Release();
            }
        }
        else if (numMessages == 0) {
            std::cout << "No messages available." << std::endl;
        }
        else {
            std::cerr << "Error receiving messages: " << numMessages << std::endl;
        }
    }

    bool done=false;

private:
    // CCallback member for LobbyChatUpdate_t
    CCallback<dataTransfer, SteamNetworkingMessagesSessionFailed_t> m_steamNetworkingMessagesSessionFailed;
    CCallback<dataTransfer, SteamNetworkingMessagesSessionRequest_t> m_steamNetworkingMessagesSessionRequest;
};