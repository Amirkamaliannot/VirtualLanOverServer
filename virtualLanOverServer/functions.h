#pragma once
#include <iostream>
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

extern std::wstring serverIP;
extern WintunManager wintunManager;


void callbackLitentToInterface(BYTE* packet, DWORD size);
void callbackLiteningToSerser(BYTE* packet, DWORD size);


std::string GetVolumeSerialNumber();

std::string getIP();