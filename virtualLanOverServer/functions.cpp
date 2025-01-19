#include "functions.h"


WintunManager wintunManager(L"AmirVPN", L"10.10.10.10", L"255.0.0.0");

void callbackLitentToInterface(BYTE* packet, DWORD size)
{
    Packet temp = Packet(packet, false, size);


}
void callbackLiteningToSerser(BYTE* packet, DWORD size)
{
    wintunManager.sendPacket(packet, size);
};
