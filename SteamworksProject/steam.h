#pragma once
#include <iostream>
#include <steam/steam_api.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include "SteamP2PConnection.h"

using namespace std;

struct steamUser
{
    CSteamID SteamID;
    string Username;
    EPersonaState State;
    string IP;
    bool connection = false;
};


class Steam {

public:
    
    DataTransfer* DT;

    Steam() :m_CallbackLobbyChatUpdate(this, &Steam::OnLobbyChatUpdate) {
        if (!SteamAPI_Init()) {
            std::cerr << "Failed to initialize Steam API. Make sure Steam is running.\n";
        }

        userID = SteamUser()->GetSteamID();
        DT = new DataTransfer();
        std::thread callbackThread(&Steam::callbackLoop, this);
        callbackThread.detach();
        steamLoop();
    }

    ~Steam() {
        END = true;
        SteamAPI_Shutdown();
    }

    void steamLoop();
    void callbackLoop();
    void runtime();
    string getUsername();
    CSteamID getUserID();
    CSteamID getLobbyID();
    string getIP();
    CSteamID getUserbyIP(string ip);
    vector<steamUser> getFriendsList();
    void CreateLobby(int capacity);
    void DeleteLobby();
    void LeaveCurrentLobby(CSteamID lobbyID);
    CSteamID GetCurrentLobby();
    vector<steamUser> ListLobbyMembers();
    void updateListLobbyMembers(CSteamID lobbyID);
    bool IsUserInLobby(CSteamID lobbyID, CSteamID userID);
    bool isLobbyValid(CSteamID lobbyID);
    int getLobbyMemberCount(CSteamID lobbyID);
    void EnterLobby(CSteamID lobbyID);
    void SearchLobbies();
    bool SendDataToUser(CSteamID targetUser, const BYTE* data, uint32 dataSize);
    void ListenForData(void(*callback)(BYTE*, DWORD));
    void startListening(void (*callback)(BYTE*, DWORD));
    bool isLobbyCreated() {return isLobbyCreated_m;}
    bool isSearchCreated();
    bool isJoinDone();
    string convertUserIdToIp(CSteamID user);

private:
    CSteamID userID;
    vector<steamUser> lobbyMemberList;
    CSteamID LobbyID = k_steamIDNil;
    CCallResult<Steam, LobbyCreated_t> m_LobbyCreated;
    CCallResult<Steam, LobbyMatchList_t> m_CallbackLobbyMatchList;
    CCallResult<Steam, LobbyEnter_t> m_LobbyEnteredCallback;
    CCallback<Steam, LobbyChatUpdate_t> m_CallbackLobbyChatUpdate;

    //states
    bool isLobbyCreated_m = false;
    bool isSearchDone_m= false;
    bool isJoinDone_m= false;
    bool END = false;

    //callbacks
    void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
    void OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
    void OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback);

};
