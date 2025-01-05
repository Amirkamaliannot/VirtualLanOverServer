// SteamworksProject.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "functions.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include "SteamP2PConnection.h"
#include <vector>

int main() {

    std::cout << "start listening on interface\n";
    wintunManager.startListening(callbackLitentToInterface);

    std::cout << "start listening on steam\n";
    steam.startListening(callbackLiteningToSteam);


    while (true) {
        std::cout << "\nEnter command: ";
        double a;
        std::cin >> a;
        //system("cls");

        if (a == 2) {
            std::vector<steamUser> list = steam.getFriendsList();
            for (int i = 0; i < list.size(); i++) {
                std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].Username << " | " << list[i].State<< "\n";
            }
        }

        if (a == 3) {
            std::cout << "creating.. \n";
            steam.CreateLobby(4);
        }

        if (a == 4) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            steam.LeaveCurrentLobby(steamID);
        }

        if (a == 5) {
            steam.SearchLobbies();
        }

        if (a == 6) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            steam.EnterLobby(steamID);
        }        
        
        if (a == 9) {
            std::vector<steamUser> list = steam.ListLobbyMembers();
            std::cout << "LobbyID:" << steam.getLobbyID().ConvertToUint64()<< "\n";
            for (int i = 0; i < list.size(); i++) {
                std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].Username << " | " << list[i].IP << "\n";
            }
        }
            
        if (a == 0) {
            break;
        }
    }
    return 1;
}

