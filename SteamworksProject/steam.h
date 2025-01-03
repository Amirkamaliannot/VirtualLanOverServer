#pragma once

#include <iostream>
#include <steam/steam_api.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;


struct steamUser
{
    CSteamID SteamID;
    string Username;
    EPersonaState State;

};


class Steam {

public:
    Steam() {

        if (!SteamAPI_Init()) {
            std::cerr << "Failed to initialize Steam API. Make sure Steam is running.\n";
        }
    }
    ~Steam() {
        SteamAPI_Shutdown();
    }

    string getUsername() {
        const char* userName = SteamFriends()->GetPersonaName();
        string str(userName);
        return str;
    }    
    
    CSteamID getUserID() {
        CSteamID userID= SteamUser()->GetSteamID();
        return userID;
    }

    vector<steamUser> getFriendsList();


    void CreateLobby(int capacity) {
        SteamAPICall_t call = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, capacity);
        m_LobbyCreated.Set(call, this, &Steam::OnLobbyCreated);
    }

    void DeleteLobby() {
        if (LobbyID.IsValid() && SteamMatchmaking()->GetLobbyOwner(LobbyID) == SteamUser()->GetSteamID()) {
            SteamMatchmaking()->LeaveLobby(LobbyID);
            std::cout << "Lobby deleted successfully." << std::endl;
        }
        else {
            std::cout << "Cannot delete the lobby; you are not the owner or lobby is invalid." << std::endl;
        }
    }

    void LeaveCurrentLobby(CSteamID lobbyID){
        if (lobbyID.IsValid()) {
            std::cout << "Leaving lobby: " << lobbyID.ConvertToUint64() << std::endl;
            SteamMatchmaking()->LeaveLobby(lobbyID);
            lobbyID = k_steamIDNil; // Reset the current lobby ID
        }
        else {
            std::cout << "You are not in a lobby to leave!" << std::endl;
        }
    }

    CSteamID GetCurrentLobby() {
        if (LobbyID.IsValid()) {
            return LobbyID;
        }
        else {
            std::cout << "You are not currently in a lobby!" << std::endl;
            return k_steamIDNil;
        }
    }


    vector<steamUser> ListLobbyMembers(CSteamID lobbyID) {

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

    bool IsUserInLobby(CSteamID lobbyID, CSteamID userID){
        vector<steamUser> lobbyMembers = ListLobbyMembers(lobbyID);
        for (int i = 0; i < lobbyMembers.size(); ++i) {
            if (lobbyMembers[i].SteamID == userID) {
                return true;
            }
        }
        return false;
    }


    bool isLobbyValid(CSteamID lobbyID) {
        if (!SteamMatchmaking()->RequestLobbyData(lobbyID)) {
            std::cout << "Failed to request lobby data for ID: " << lobbyID.ConvertToUint64() << std::endl;
            return false;
        }
        return true;
    }

    int getLobbyMemberCount(CSteamID lobbyID) {
        // Check if lobby ID is valid
        if (!lobbyID.IsValid()) {
            std::cout << "Invalid lobby ID!" << std::endl;
            return 0;
        }

        // Get the number of members in the lobby
        int memberCount = SteamMatchmaking()->GetNumLobbyMembers(lobbyID);
        return memberCount;
    }


    void EnterLobby(CSteamID lobbyID) {
        // Attempt to join the lobby
        SteamAPICall_t hSteamAPICall = SteamMatchmaking()->JoinLobby(lobbyID);
        m_LobbyEnteredCallback.Set(hSteamAPICall, this, &Steam::OnLobbyEnter);
    }


    void SearchLobbies() {
        // Request the lobby list
        SteamMatchmaking()->AddRequestLobbyListStringFilter("code", "b4w48tfRds479", k_ELobbyComparisonEqual);
        SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
        m_CallbackLobbyMatchList.Set(hSteamAPICall, this, &Steam::OnLobbyMatchList);
    }


    bool isLobbyCreated() {
        return isLobbyCreated_m;
    }    
    bool isSearchCreated() {
        bool temp = isSearchDone_m;
        isSearchDone_m = false;
        return temp;
    }    
    bool isJoinDone() {
        bool temp = isJoinDone_m;
        isJoinDone_m = false;
        return temp;
    }


    bool SendDataToUser(CSteamID targetUser, const std::string& message) {
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

    void ListenForData(void(*callback)(BYTE*, DWORD)) {
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


private:
    CSteamID LobbyID = k_steamIDNil;
    CCallResult<Steam, LobbyCreated_t> m_LobbyCreated;
    CCallResult<Steam, LobbyMatchList_t> m_CallbackLobbyMatchList;
    CCallResult<Steam, LobbyEnter_t> m_LobbyEnteredCallback;
    bool isLobbyCreated_m = false;
    bool isSearchDone_m= false;
    bool isJoinDone_m= false;

    void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure) {
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

    void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure) {
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

    void OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure) {
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


};




// Class to manage lobby creation
class LobbyManager {
public:
    LobbyManager() {
        // Register the callback manually
        m_CallbackLobbyCreated.Register(this, &LobbyManager::OnLobbyCreated);
    }

    // Function to request lobby creation
    void CreateLobby() {
        std::cout << "Requesting to create a lobby...\n";

        // Create a lobby with public visibility and a maximum of 4 players
        SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 4);
    }

    bool isLobbyCreated() {
        return isCreated;
    }

private:
    // Callback function to handle lobby creation result
    void OnLobbyCreated(LobbyCreated_t* pCallback) {
        if (pCallback->m_eResult != k_EResultOK) {
            std::cerr << "Failed to create lobby. Error code: " << pCallback->m_eResult << "\n";
            return;
        }

        std::cout << "Lobby created successfully!\n";
        isCreated = true;
        std::cout << "Lobby ID: " << pCallback->m_ulSteamIDLobby << "\n";

        // Save the lobby ID for further use
        m_LobbyID = CSteamID(pCallback->m_ulSteamIDLobby);

        SteamMatchmaking()->SetLobbyData(m_LobbyID, "game_mode", "deathmatch");
        SteamMatchmaking()->SetLobbyData(m_LobbyID, "region", "NA");
        SteamMatchmaking()->SetLobbyData(m_LobbyID, "code", "b44");
        SteamMatchmaking()->SetLobbyData(m_LobbyID, "max_players", "4");
    }


    bool isCreated = false;
    CSteamID m_LobbyID; // Store the created lobby ID

    CCallback<LobbyManager, LobbyCreated_t> m_CallbackLobbyCreated{
        this, &LobbyManager::OnLobbyCreated
    };
};


//int tempSteam() {
//
//
//
//    // Get the number of friends
//    int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
//    std::cout << "You have " << friendCount << " friends.\n";
//
//    // Loop through each friend
//    for (int i = 0; i < friendCount; ++i) {
//        // Get the Steam ID of the friend
//        CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
//
//        // Get the friend's name
//        const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
//
//        // Get the friend's current status
//        EPersonaState friendState = SteamFriends()->GetFriendPersonaState(friendSteamID);
//
//        // Display information
//        std::cout << "Friend " << i + 1 << ": " << friendName << " (Steam ID: " << friendSteamID.ConvertToUint64() << ")\n";
//        switch (friendState) {
//        case k_EPersonaStateOnline:
//            std::cout << " - Status: Online\n";
//            break;
//        case k_EPersonaStateOffline:
//            std::cout << " - Status: Offline\n";
//            break;
//        case k_EPersonaStateBusy:
//            std::cout << " - Status: Busy\n";
//            break;
//        case k_EPersonaStateAway:
//            std::cout << " - Status: Away\n";
//            break;
//        case k_EPersonaStateSnooze:
//            std::cout << " - Status: Snooze\n";
//            break;
//        case k_EPersonaStateLookingToTrade:
//            std::cout << " - Status: Looking to Trade\n";
//            break;
//        case k_EPersonaStateLookingToPlay:
//            std::cout << " - Status: Looking to Play\n";
//            break;
//        default:
//            std::cout << " - Status: Unknown\n";
//            break;
//        }
//    }
//
//
//
//    // Create an instance of the LobbyManager
//    LobbyManager lobbyManager;
//    // Request to create a lobby
//    lobbyManager.CreateLobby();
//
//    // Main loop to process Steam API callbacks
//    while (true) {
//        SteamAPI_RunCallbacks();
//
//        // Add a delay to prevent busy-waiting
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        std::cout << "...\n";
//        if (lobbyManager.isLobbyCreated()) {
//            break;
//        }
//    }



//    return 0;
//}