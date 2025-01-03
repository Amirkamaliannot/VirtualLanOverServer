#include "Packet.h"
#include <winsock2.h>  // For inet_ntoa
#include <windows.h>
#include <vector>
#include <iomanip>
#include <pcap.h>
#include <iostream>

void Packet::create(bool ETHERNET_FRAME){

    ETHERNET_HEADER_SIZE = 14 * ETHERNET_FRAME;
    if (!raw_pkt) {
        throw std::invalid_argument("Packet data cannot be null.");
    }

    // Extract IP header
    ip_header = (struct IP_header*)(raw_pkt + ETHERNET_HEADER_SIZE);
    std::cout << *(BYTE*)(raw_pkt + ETHERNET_HEADER_SIZE);

    // Validate IHL
    if (ip_header->ihl < 5) {
        //throw std::runtime_error("Invalid IP header length.");
    }
    // Calculate IP header size and total length
    ip_header_length = ip_header->ihl * 4;
    packet_length = ntohs(ip_header->total_length) + 14;


    if (packet_length < ip_header_length) {
        throw std::runtime_error("Invalid total length.");
    }

    // Determine the protocol
    if (ip_header->protocol == 6) { // TCP
        tcp_header = reinterpret_cast<struct TCP_header*>(raw_pkt + ETHERNET_HEADER_SIZE + ip_header_length);

        std::cout << (tcp_header->offset_reserved >> 4) * 4 << "\n";
        trans_header_length = (tcp_header->offset_reserved >> 4) * 4;
        payload = (raw_pkt + ETHERNET_HEADER_SIZE + ip_header_length + trans_header_length);
    }
    else if (ip_header->protocol == 17) { // UDP
        udp_header = reinterpret_cast<struct UDP_header*>(raw_pkt + ETHERNET_HEADER_SIZE + ip_header_length);
        trans_header_length = 8;
        payload = raw_pkt + ETHERNET_HEADER_SIZE + ip_header_length + trans_header_length;
    }

    payload_length = packet_length - (ETHERNET_HEADER_SIZE + ip_header_length + trans_header_length);
    std::cout << "payload size:" << payload_length << "\n";


    // Convert source and destination IP addresses to strings
    inet_ntop(AF_INET, &ip_header->src_addr, src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->dst_addr, dst_ip, INET_ADDRSTRLEN);

}

void Packet::printIP()
{
    std::cout << "Source IP: " << src_ip << " || " << "Destination IP: " << dst_ip << std::endl;
}
