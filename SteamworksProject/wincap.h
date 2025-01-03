#pragma once
#include <pcap.h>
#include <iostream>
#include <iomanip>
#include <winsock2.h>  // For inet_ntoa
#include <windows.h>
#include <vector>
#include <functional>
#include "Packet.h"


struct CallbackData {
    std::function<void(Packet)> callback; // Function to handle results
    std::string extra_info; // Some extra information, if needed
};

void packet_handler(u_char* user, const struct pcap_pkthdr* header, const u_char* packet);

int start_capturing(const std::string& filter_exp, void (*callback)(Packet));

int capture_packets(const char* dev_name, const std::string& filter_exp, void (*callback)(Packet));


