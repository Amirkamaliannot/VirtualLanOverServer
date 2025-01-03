#include <pcap.h>
#include <iostream>
#include <iomanip>
#include <winsock2.h>  // For inet_ntoa
#include <windows.h>
#include <thread>
#include "wincap.h"
#include <vector>
#include <functional>
#include "Packet.h"


// Function to capture packets on a specific interface
int capture_packets(const char* dev_name, const std::string& filter_exp, void (*callback)(Packet)) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(dev_name, 65536, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Error opening device " << dev_name << ": " << errbuf << std::endl;
        return 1;
    }

    struct bpf_program filter;
    if (pcap_compile(handle, &filter, filter_exp.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        std::cerr << "Error compiling filter: " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    if (pcap_setfilter(handle, &filter) == -1) {
        std::cerr << "Error setting filter: " << pcap_geterr(handle) << std::endl;
        return 1;
    }

    //std::cout << "Listening on device: " << dev_name << std::endl;

    CallbackData cb_data{ callback, "MyCaptureSession" };

    // Start capturing packets
    pcap_loop(handle, 0, packet_handler, reinterpret_cast<u_char*>(&cb_data));

    // Cleanup
    pcap_close(handle);
}

// Callback function for packet processing
void packet_handler(u_char* user, const struct pcap_pkthdr* header, const u_char* packet) {

    CallbackData* cb_data = reinterpret_cast<CallbackData*>(user);
    u_char* buffer = new u_char[header->len];
    std::memcpy(buffer, packet, header->len);

    Packet pkt(buffer);

    if (cb_data->callback) {
        cb_data->callback(pkt);
    }
}


int start_capturing(const std::string& filter_exp, void (*callback)(Packet)) {
    pcap_if_t* alldevs;
    char errbuf[PCAP_ERRBUF_SIZE];

    // Get the list of devices
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << std::endl;
        return 1;
    }

    // Start capturing packets on all devices
    std::vector<std::thread> threads;
    for (pcap_if_t* dev = alldevs; dev != nullptr; dev = dev->next) {
        if (!dev->name) continue; // Skip devices without a name
        threads.emplace_back(capture_packets, dev->name, filter_exp, callback);
    }

    // Wait for all threads to finish (will run indefinitely until you interrupt)
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    // Free the device list
    pcap_freealldevs(alldevs);

    return 1;
}

//start_capturing("dst host 192.168.20.25 || src host 192.168.20.25", callback);
