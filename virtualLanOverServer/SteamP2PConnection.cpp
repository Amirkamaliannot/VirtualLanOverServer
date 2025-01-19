#include "SteamP2PConnection.h"

void DataTransfer::ProcessIncomingMessages(void(*callback)(BYTE*, DWORD))
{
    ISteamNetworkingMessage* pMessages[MAX_MESSAGES] = { nullptr };

    // Receive messages on the specified channel
    int numMessages = m_networkingMessages->ReceiveMessagesOnChannel(
        CHANNEL_ID,
        pMessages,
        MAX_MESSAGES
    );
    if (numMessages > 0) {
        //std::cout << "Received " << numMessages << " messages." << std::endl;

        for (int i = 0; i < numMessages; ++i) {
            ISteamNetworkingMessage* pMessage = pMessages[i];
            callback((BYTE*)pMessage->m_pData, pMessage->m_cbSize);
            // Release the message to free its resources
            pMessage->Release();
        }
    }
}

EResult DataTransfer::SendMessageW(CSteamID userID, const void* data, uint32 dataSize, int flags)
{
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
    return result;
}

void DataTransfer::onSessionFailed(SteamNetworkingMessagesSessionFailed_t* callback)
{
    std::cout << "onSessionFailed\n";
}

void DataTransfer::onSessionRequest(SteamNetworkingMessagesSessionRequest_t* callback)
{
    // Validate the remote identity
    SteamNetworkingIdentity identityRemote;
    identityRemote.SetSteamID(callback->m_identityRemote.GetSteamID());

    std::cout << "SteamID: " << identityRemote.GetSteamID().ConvertToUint64() << std::endl;

    // Attempt to accept the session
    bool result = m_networkingMessages->AcceptSessionWithUser(identityRemote);
    if (result) {
        //std::cout << "SessionAccepted\n";
    }
    else {
        std::cerr << "SessionNOTAccepted: Check remote user, network, or state.\n";
    }
}