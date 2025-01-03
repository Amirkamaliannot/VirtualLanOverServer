// SteamworksProject.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <steam/steam_api.h>
#include <thread>
#include <chrono>
#include <iomanip>
#include <winsock2.h> 
#include <windows.h>
#include "steam.h"
#include "winperf.h"
//#include "wincap.h"
#include <thread>
#include <vector>
#include "Packet.h"
#include "wintun.h"
#include <iphlpapi.h>
#include "WintunManager.h"
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")


void callbackLitentToInterface(BYTE* packet, DWORD size) {

    Packet temp = Packet(packet, false, size);
    temp.printIP();

    std::cout << "ok\n";
};

void callbackLiteningToSteam(BYTE* packet, DWORD size) {

    for (int i = 0; i < (int)size; i++) {
        std::cout << (char)packet[i];
    }
    std::cout << "\n";


}


int main() {


    //WintunManager wintunManager(L"AmirVPN", L"10.20.30.40", L"255.0.0.0");
    //wintunManager.startListening(callback);


    Steam steam;

    while (true) {
        std::cout << "\nEnter command: ";
        double a;
        std::cin >> a;
        //system("cls");

        if (a == 1) {
            //std::cout << steam.getUsername() <<"\n";
            //std::cout << steam.getUserID().ConvertToUint64() <<"\n";

            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            std::cout << steam.convertUserIdToIp(steamID);

        }
        if (a == 2) {
            std::vector<steamUser> list = steam.getFriendsList();
            for (int i = 0; i < list.size(); i++) {
                std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].Username << " | " << list[i].State<< "\n";
            }
        }

        if (a == 3) {

            std::cout << "creating.. \n";
            steam.CreateLobby(4);
            while (true) {
                SteamAPI_RunCallbacks();

                // Add a delay to prevent busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "...\n";
                if (steam.isLobbyCreated()) {
                    break;
                }
            }
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
            while (true) {
                SteamAPI_RunCallbacks();

                // Add a delay to prevent busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                std::cout << "...\n";
                if (steam.isSearchCreated()) {
                    break;
                }
            }
        }

        if (a == 6) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            steam.EnterLobby(steamID);
        }        
        
        if (a == 7) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            if (steam.isLobbyValid(steamID)) {
                std::cout << "valid \n";
            
            }else {

                std::cout << "not valid!! \n" ;
            };             
            if (steam.isLobbyValid(steam.GetCurrentLobby())){
                std::cout << "valid \n";
            
            }else {

                std::cout << "not valid!! \n";
            };
        }        
        if (a == 8) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64); 

            std::cout << "Enter a user ID (64-bit integer): ";
            std::cin >> steamID64;
            CSteamID userID(steamID64);
            if (steam.IsUserInLobby(steamID,userID)) {
                cout << "ok\n";
            }
            else {
                cout << "0\n";
            }

        }        
        if (a == 9) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64); 

            std::vector<steamUser> list = steam.ListLobbyMembers(steamID);
            for (int i = 0; i < list.size(); i++) {
                std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].Username << " | " << list[i].IP << "\n";
            }

        }

        if (a == 10) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);

            if (steam.SendDataToUser(steamID, "hellow world")) {
                std::cout << "sent\n";
            };
        }

        if (a == 11) {
            std::cout << "listening..\n";
            steam.ListenForData(callbackLiteningToSteam);
        }

        if (a == 0) {
            break;
        }

    }
    return 1;
}

