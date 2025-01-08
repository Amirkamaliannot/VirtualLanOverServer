#include "functions.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include "SteamP2PConnection.h"
#include <vector>
using std::setw;

int main() {

    std::cout << "start listening on interface\n";
    wintunManager.startListening(callbackLitentToInterface);

    std::cout << "start listening on steam\n";
    steam.startListening(callbackLiteningToSteam);

    std::cout << "\n\n";
    std::cout << "1 - Get friends list\n";
    std::cout << "2 - Create and join a lobby\n";
    std::cout << "3 - Leave a lobby\n";
    std::cout << "4 - Search and join first lobby\n";
    std::cout << "5 - Enter a lobby (by address)\n";
    std::cout << "6 - Get your lobby members list\n";


    while (true) {

        std::cout << "\nEnter command: ";

        double a;
        std::cin >> a;

        if (a == 1) {
            std::vector<steamUser> list = steam.getFriendsList();
            for (int i = 0; i < list.size(); i++) {
                std::cout << "ID:" << list[i].SteamID.ConvertToUint64() << " | " << list[i].Username << " | " << list[i].State<< "\n";
            }
        }

        if (a == 2) {
            std::cout << "creating.. \n";
            steam.CreateLobby(4);
        }

        if (a == 3) {
            steam.LeaveCurrentLobby(steam.getUserID());
        }

        if (a == 4) {
            steam.SearchLobbies();
        }

        if (a == 5) {
            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            steam.EnterLobby(steamID);
        }        
        
        if (a == 6) {
            std::vector<steamUser> list = steam.ListLobbyMembers();
            std::cout << "LobbyID: " << steam.getLobbyID().ConvertToUint64()<< "\n\n";
            // Print column headers
            cout << left << setw(20) << "ID"
                << setw(20) << "Username"
                << setw(20) << "IP"
                << setw(10) << "Ping(ms)" << "\n";

            cout << string(70, '-') << "\n"; // Separator line
            for (int i = 0; i < list.size(); i++) {

                cout << left << setw(20) << list[i].SteamID.ConvertToUint64()
                    << setw(20) << list[i].Username
                    << setw(20) << list[i].IP
                    << setw(4) << steam.getPing(list[i].SteamID) << "ms\n";
            }
        }
        if (a == 7) {

            std::cout << "Enter a Steam ID (64-bit integer): ";
            uint64_t steamID64;
            std::cin >> steamID64;
            CSteamID steamID(steamID64);
            steam.getPing(steamID);
        }

        if (a == 8) {
            if (steam.IsSteamConnected()) {
                std::cout << "Connected";
            }
            else {
                std::cout << "not Connected";
            }
            
        }
            
        if (a == 0) {
            break;
        }
    }
    return 1;
}

