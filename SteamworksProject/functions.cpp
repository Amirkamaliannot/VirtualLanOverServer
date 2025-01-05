#include "functions.h"


Steam steam;
std::wstring steamIP = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(steam.getIP());
WintunManager wintunManager(L"AmirVPN", steamIP, L"255.0.0.0");

void callbackLitentToInterface(BYTE* packet, DWORD size)
{
    Packet temp = Packet(packet, false, size);
    string dst_ip = temp.dst_ip;
    CSteamID steamID = steam.getUserbyIP(dst_ip);

    if (steamID != k_steamIDNil) {
        steam.SendDataToUser(steamID, packet, size);
    }
}
void callbackLiteningToSteam(BYTE* packet, DWORD size)
{
    wintunManager.sendPacket(packet, size);
};

;
