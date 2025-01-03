
#include <steam/steam_api.h>
#include <iostream>

//class LobbyManager {
//public:
//    LobbyManager() {
//        // Register the callback manually
//        m_callback_OnLobbyCreated.Register(this, &LobbyManager::OnLobbyCreated);
//    }
//
//    void CreateLobby() {
//        SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 4);
//        std::cout << "Lobby creation requested...\n";
//    }
//
//    void OnLobbyCreated(LobbyCreated_t* pCallback) {
//        if (pCallback->m_eResult != k_EResultOK) {
//            std::cerr << "Failed to create lobby.\n";
//            return;
//        }
//
//        std::cout << "Lobby created successfully! Lobby ID: "
//            << pCallback->m_ulSteamIDLobby << "\n";
//
//        lobbyID = CSteamID(pCallback->m_ulSteamIDLobby);
//    }
//
//    CSteamID GetLobbyID() const { return lobbyID; }
//
//private:
//    CSteamID lobbyID;
//
//    // Manual callback declaration
//    CCallback<LobbyManager, LobbyCreated_t> m_callback_OnLobbyCreated;
//};