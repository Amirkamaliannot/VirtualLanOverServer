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

        userID = SteamUser()->GetSteamID();

        SearchLobbies();
        while (true) {
            SteamAPI_RunCallbacks();
            // Add a delay to prevent busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "...\n";
            if (isSearchCreated()) {
                break;
            }
        }
        if (LobbyID == k_steamIDNil) {
            CreateLobby(6);
            while (true) {
                SteamAPI_RunCallbacks();
                // Add a delay to prevent busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                std::cout << "...\n";
                if (isLobbyCreated()) {
                    break;
                }
            }
        }

        std::vector<steamUser> list = ListLobbyMembers(LobbyID);
        for (int i = 0; i < list.size(); i++) {
            std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].IP << " | " << list[i].Username <<  "\n";
        }



    }
    ~Steam() {
        END = true;
        SteamAPI_Shutdown();
    }



    string getUsername();
    CSteamID getUserID();
    string getIP();
    CSteamID getUserbyIP(string ip);
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
    bool SendDataToUser(CSteamID targetUser, const BYTE* data, uint32 dataSize);
    void ListenForData(void(*callback)(BYTE*, DWORD));
    void startListening(void (*callback)(BYTE*, DWORD));
    bool isLobbyCreated() {return isLobbyCreated_m;}
    bool isSearchCreated();
    bool isJoinDone();

    string convertUserIdToIp(CSteamID user);


private:
    CSteamID userID;
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