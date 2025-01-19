#pragma once
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <winsock2.h> 
#include <windows.h>
#include "winperf.h"
#include <thread>
#include <vector>
#include "Packet.h"
#include "wintun.h"
#include <codecvt>
#include <iphlpapi.h>
#include <unordered_map>
#include "WintunManager.h"
#include "Server.h"
#include "Lobby.h"

extern std::wstring serverIP;
extern Server server;
extern Lobby lobby;
extern WintunManager wintunManager;

void callbackLitentToInterface(BYTE* packet, DWORD size);
void callbackLiteningToServer(BYTE* packet, DWORD size);

std::string readingIp();


std::string GetVolumeSerialNumber();

std::string getIP();

bool startsWithCommand(const BYTE* data, size_t dataSize);