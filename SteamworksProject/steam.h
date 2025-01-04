#pragma once

#include <iostream>
#include <steam/steam_api.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <windows.h>
#include <thread>

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
        userID = SteamUser()->GetSteamID();
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

    void runtime() {

        while (!END) {
            SearchLobbies();
            while (true) {
                SteamAPI_RunCallbacks();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (isSearchCreated()) {
                    break;
                }
            }
            if (LobbyID == k_steamIDNil) {
                CreateLobby(6);
                while (true) {
                    SteamAPI_RunCallbacks();
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    if (isLobbyCreated()) {
                        break;
                    }
                }
            }
            updateListLobbyMembers(LobbyID);
            lobbyMemberList = ListLobbyMembers();
            for (int i = 0; i < lobbyMemberList.size(); i++) {
                std::cout << "ID: " << lobbyMemberList[i].SteamID.ConvertToUint64() << " | " << lobbyMemberList[i].IP << " | " << lobbyMemberList[i].Username << "\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
    };

    string getUsername();
    CSteamID getUserID();
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
    bool isLobbyCreated_m = false;
    bool isSearchDone_m= false;
    bool isJoinDone_m= false;
    bool END = false;

    void OnLobbyCreated(LobbyCreated_t* pCallback, bool bIOFailure);
    void OnLobbyMatchList(LobbyMatchList_t* pLobbyMatchList, bool bIOFailure);
    void OnLobbyEnter(LobbyEnter_t* pCallback, bool bIOFailure);
};