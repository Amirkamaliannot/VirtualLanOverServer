#include "steam.h"

vector<steamUser> Steam::getFriendsList()
{
    int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

    vector<steamUser> friendsList;

    for (int i = 0; i < friendCount; ++i) {
        // Get the Steam ID of the friend
        CSteamID friendSteamID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
        // Get the friend's name
        const char* friendName = SteamFriends()->GetFriendPersonaName(friendSteamID);
        // Get the friend's current status
        EPersonaState friendState = SteamFriends()->GetFriendPersonaState(friendSteamID);

        steamUser friends{ friendSteamID, friendName, friendState };
        friendsList.push_back(friends);
    };
    return friendsList;
};
