#include "steam.h"

string Steam::getUsername()
{
    const char* userName = SteamFriends()->GetPersonaName();
    string str(userName);
    return str;
}

CSteamID Steam::getUserID()
{
    CSteamID userID = SteamUser()->GetSteamID();
    return userID;
}

vector<steamUser> Steam::getFriendsList()
{
    int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

    vector<steamUser> friendsList;

    for (int i = 0; i < friendCount; ++i) {
        // Get the Steam ID of the friend
        CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
        // Get the friend's name
        const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
        // Get the friend's current status
        EPersonaState friendState = SteamFriends()->GetFriendPersonaState(friendSteamID);

        steamUser friends{ friendSteamID, friendName, friendState };
        friendsList.push_back(friends);
    };
    return friendsList;
}
void Steam::CreateLobby(int capacity)
{
    SteamAPICall_t call = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, capacity);
    m_LobbyCreated.Set(call, this, &Steam::OnLobbyCreated);
}
void Steam::DeleteLobby()
{
    if (LobbyID.IsValid() && SteamMatchmaking()->GetLobbyOwner(LobbyID) == SteamUser()->GetSteamID()) {
        SteamMatchmaking()->LeaveLobby(LobbyID);
        std::cout << "Lobby deleted successfully." << std::endl;
    }
    else {
        std::cout << "Cannot delete the lobby; you are not the owner or lobby is invalid." << std::endl;
    }
}

void Steam::LeaveCurrentLobby(CSteamID lobbyID)
{
    if (lobbyID.IsValid()) {
        std::cout << "Leaving lobby: " << lobbyID.ConvertToUint64() << std::endl;
        SteamMatchmaking()->LeaveLobby(lobbyID);
        lobbyID = k_steamIDNil; // Reset the current lobby ID
    }
    else {
        std::cout << "You are not in a lobby to leave!" << std::endl;
    }
}

CSteamID Steam::GetCurrentLobby()
{
    if (LobbyID.IsValid()) {
        return LobbyID;
    }
    else {
        std::cout << "You are not currently in a lobby!" << std::endl;
        return k_steamIDNil;
    }
}

vector<steamUser> Steam::ListLobbyMembers(CSteamID lobbyID)
{
    vector<steamUser> list;

    if (!lobbyID.IsValid()) {
        std::cout << "Invalid lobby ID!" << std::endl;
        return list;
    }

    int memberCount = SteamMatchmaking()->GetNumLobbyMembers(lobbyID);

    for (int i = 0; i < memberCount; ++i) {
        CSteamID memberID = SteamMatchmaking()->GetLobbyMemberByIndex(lobbyID, i);
        const char* memberName = SteamFriends()->GetFriendPersonaName(memberID);
        steamUser user{ memberID, memberName, k_EPersonaStateOnline };

        list.push_back(user);
    }
}

bool Steam::IsUserInLobby(CSteamID lobbyID, CSteamID userID)
{
    vector<steamUser> lobbyMembers = ListLobbyMembers(lobbyID);
    for (int i = 0; i < lobbyMembers.size(); ++i) {
        if (lobbyMembers[i].SteamID == userID) {
            return true;
        }
    }
    return false;
}

bool Steam::isLobbyValid(CSteamID lobbyID)
{
    if (!SteamMatchmaking()->RequestLobbyData(lobbyID)) {
        std::cout << "Failed to request lobby data for ID: " << lobbyID.ConvertToUint64() << std::endl;
        return false;
    }
    return true;
}

int Steam::getLobbyMemberCount(CSteamID lobbyID)
{
    if (!lobbyID.IsValid()) {
        std::cout << "Invalid lobby ID!" << std::endl;
        return 0;
    }

    int memberCount = SteamMatchmaking()->GetNumLobbyMembers(lobbyID);
    return memberCount;
}

void Steam::EnterLobby(CSteamID lobbyID)
{
    // Attempt to join the lobby
    SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(lobbyID);
    m_LobbyEnteredCallback.Set(hSteamAPICall, this, &Steam::OnLobbyEnter);
}

void Steam::SearchLobbies()
{
    // Request the lobby list
    SteamMatchmaking()->AddRequestLobbyListStringFilter("code", "b4w48tfRds479", k_ELobbyComparisonEqual);
    SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
    m_CallbackLobbyMatchList.Set(hSteamAPICall, this, &Steam::OnLobbyMatchList);
}

bool Steam::SendDataToUser(CSteamID targetUser, const std::string& message)
{
    // Convert the message to a byte array
    const void* pData = message.c_str();
    uint32 dataSize = static_cast<uint32>(message.size());

    // Send the message
    bool success = SteamNetworking()->SendP2PPacket(
        targetUser,          // Target user's SteamID
        pData,               // Pointer to the message data
        dataSize,            // Size of the message
        k_EP2PSendReliable,  // Reliable send type
        0                    // Channel to use (default is 0)
    );

    if (!success) {
        std::cerr << "Failed to send message to user." << std::endl;
    }

    return success;
}

void Steam::ListenForData(void(*callback)(BYTE*, DWORD))
{
    uint32 messageSize;

    // Check if there are packets available
    while (SteamNetworking()->IsP2PPacketAvailable(&messageSize)) {
        char* buffer = new char[messageSize];  // Allocate memory for the message
        uint32 bytesRead;
        CSteamID sender;

        // Read the packet
        if (SteamNetworking()->ReadP2PPacket(buffer, messageSize, &bytesRead, &sender)) {
            // Process the message
            callback((BYTE*)buffer, (DWORD)bytesRead);
        }

        delete[] buffer;  // Clean up allocated memory
    }
}

bool Steam::isSearchCreated()
{
    bool temp = isSearchDone_m;
    isSearchDone_m = false;
    return temp;
}

bool Steam::isJoinDone()
{
    bool temp = isJoinDone_m;
    isJoinDone_m = false;
    return temp;
}

void Steam::OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure)
{
    if (bIOFailure || pCallback->m_eResult != k_EResultOK) {
        std::cout << "Failed to create lobby." << std::endl;
        return;
    }

    LobbyID = pCallback->m_ulSteamIDLobby;
    isLobbyCreated_m = true;
    std::cout << "Lobby created: " << LobbyID.ConvertToUint64() << std::endl;
    SteamMatchmaking()->SetLobbyData(LobbyID, "game_mode", "deathmatch");
    SteamMatchmaking()->SetLobbyData(LobbyID, "region", "AS");
    SteamMatchmaking()->SetLobbyData(LobbyID, "code", "b4w48tfRds479");
    SteamMatchmaking()->SetLobbyData(LobbyID, "max_players", "4");
    std::cout << "Lobby configed." << std::endl;
}

void Steam::OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure)
{
    if (bIOFailure) {
        std::cout << "Lobby search failed due to IO error." << std::endl;
        return;
    }

    std::cout << "Found " << pLobbyMatchList->m_nLobbiesMatching << " lobbies." << std::endl;

    // Iterate through lobbies and print their IDs
    for (int i = 0; i < (int)pLobbyMatchList->m_nLobbiesMatching; ++i) {
        CSteamID lobbyID = SteamMatchmaking()->GetLobbyByIndex(i);
        std::cout << "Lobby ID: " << lobbyID.ConvertToUint64() << " member:" << getLobbyMemberCount(lobbyID) << std::endl;
    }
    isSearchDone_m = true;
}

void Steam::OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure)
{
    if (bIOFailure) {
        std::cout << "Failed to query the lobby: IO failure." << std::endl;
        return;
    }

    if (pCallback->m_EChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess) {
        std::cout << "Lobby exists and joined successfully. Lobby ID: "
            << pCallback->m_ulSteamIDLobby << std::endl;
    }
    else {
        std::cout << "Lobby does not exist or cannot be joined. Error code: "
            << pCallback->m_EChatRoomEnterResponse << std::endl;
    }
    isJoinDone_m = true;
}