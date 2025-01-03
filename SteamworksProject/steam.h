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
    string IP;
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

    string getUsername();
    CSteamID getUserID();
    vector<steamUser> getFriendsList();
    void CreateLobby(int capacity);
    void DeleteLobby();
    void LeaveCurrentLobby(CSteamID lobbyID);
    CSteamID GetCurrentLobby();
    vector<steamUser> ListLobbyMembers(CSteamID lobbyID);
    bool IsUserInLobby(CSteamID lobbyID, CSteamID userID);
    bool isLobbyValid(CSteamID lobbyID);
    int getLobbyMemberCount(CSteamID lobbyID);
    void EnterLobby(CSteamID lobbyID);
    void SearchLobbies();
    bool SendDataToUser(CSteamID targetUser, const std::string& message);
    void ListenForData(void(*callback)(BYTE*, DWORD));
    bool isLobbyCreated() {return isLobbyCreated_m;}
    bool isSearchCreated();
    bool isJoinDone();
    string convertUserIdToIp(CSteamID user);


private:
    CSteamID LobbyID = k_steamIDNil;
    CCallResult<Steam, LobbyCreated_t> m_LobbyCreated;
    CCallResult<Steam, LobbyMatchList_t> m_CallbackLobbyMatchList;
    CCallResult<Steam, LobbyEnter_t> m_LobbyEnteredCallback;
    bool isLobbyCreated_m = false;
    bool isSearchDone_m= false;
    bool isJoinDone_m= false;

    void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
    void OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
};