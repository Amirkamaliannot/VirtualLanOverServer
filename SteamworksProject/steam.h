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

    void steamLoop() {

        std::thread steamThread(&Steam::runtime, this);
        steamThread.detach(); // Detach the thread if you don't want to join it later
    };    
    void callbackLoop() {
        while (!END) {
            SteamAPI_RunCallbacks();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    };



    void runtime() {

        while (!END) {
            SearchLobbies();
            while (!isSearchCreated()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            if (LobbyID == k_steamIDNil) {
                CreateLobby(4);
                while (!isLobbyCreated()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
            updateListLobbyMembers(LobbyID);
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
    };



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

    bool isLobbyCreated_m = false;
    bool isSearchDone_m= false;
    bool isJoinDone_m= false;
    bool END = false;

    void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
    void OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
    void OnLobbyChatUpdate(LobbyChatUpdate_t* pCallback) {

        bool userFinded = false;
        CSteamID lobbyID = CSteamID(pCallback->m_ulSteamIDLobby);
        CSteamID userChangedID = CSteamID(pCallback->m_ulSteamIDUserChanged);
        CSteamID makingChangeID = CSteamID(pCallback->m_ulSteamIDMakingChange);
        uint32 changeFlags = pCallback->m_rgfChatMemberStateChange;


        //User joined the lobby;
        if (changeFlags & k_EChatMemberStateChangeEntered) {
            for (int i = 0; i < lobbyMemberList.size(); i++) {
                if (lobbyMemberList[i].SteamID == userChangedID) {
                    userFinded = true;
                    break;
                }
            }
            if (!userFinded) {
                steamUser user{ 
                    userChangedID,
                    SteamFriends()->GetFriendPersonaName(userChangedID),
                    k_EPersonaStateOnline,
                    convertUserIdToIp(userChangedID) 
                };
                lobbyMemberList.push_back(user);
            }
            userFinded = false;
        }

        //"User left the lobby  or  User disconnected from the lobby  or  User was kicked from the lobby  or  User was banned from the lobby
        if (
            changeFlags & k_EChatMemberStateChangeLeft  ||
            changeFlags & k_EChatMemberStateChangeDisconnected ||
            changeFlags & k_EChatMemberStateChangeKicked ||
            changeFlags & k_EChatMemberStateChangeBanned
            )
        {
            for (int i = 0; i < lobbyMemberList.size(); i++) {
                if (lobbyMemberList[i].SteamID == userChangedID) {
                    lobbyMemberList.erase(lobbyMemberList.begin() + i);
                }
            }
        }
        //std::cout << "Lobby ID: " << lobbyID.ConvertToUint64() << std::endl;
        //std::cout << "User Changed: " << userChangedID.ConvertToUint64() << std::endl;
    }



    
};
