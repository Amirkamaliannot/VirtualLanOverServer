#pragma once
#include <winsock2.h>  // For inet_ntoa
#include <windows.h>
#include <vector>
#include <iomanip>
#include <pcap.h>
#include <iostream>

struct nt_header {
    uint16_t src_port;   // Source port
    uint16_t dst_port;   // Destination port
    uint16_t length;     // Length of the transport layer header (TCP/UDP)
    uint16_t checksum;   // Checksum (TCP/UDP)
    // Additional fields could be added here (e.g., sequence/ack numbers for TCP)
};

#pragma pack(push, 1)
struct IP_header {
    unsigned char ihl : 4;      // Internet Header Length
    unsigned char version : 4;  // Version
    unsigned char tos;          // Type of Service
    unsigned short total_length;// Total length
    unsigned short id;          // Identification
    unsigned short frag_offset; // Fragment offset
    unsigned char ttl;          // Time to live
    unsigned char protocol;     // Protocol
    unsigned short checksum;    // Header checksum
    unsigned int src_addr;      // Source address
    unsigned int dst_addr;      // Destination address
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TCP_header {
    uint16_t src_port;         // Source port (2 bytes)
    uint16_t dest_port;        // Destination port (2 bytes)
    uint32_t seq_number;       // Sequence number (4 bytes)
    uint32_t ack_number;       // Acknowledgment number (4 bytes)
    uint8_t offset_reserved;   // Data offset (4 bits) + Reserved (4 bits)
    uint8_t flags;             // Control flags (1 byte)
    uint16_t window;           // Window size (2 bytes)
    uint16_t checksum;         // Checksum (2 bytes)
    uint16_t urgent_pointer;   // Urgent pointer (2 bytes)
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UDP_header {
    uint16_t src_port;      // Source port (2 bytes)
    uint16_t dest_port;     // Destination port (2 bytes)
    uint16_t length;        // Length of UDP header and payload (2 bytes)
    uint16_t checksum;      // Checksum (2 bytes)
};
#pragma pack(pop)


class Packet
{
public:
    Packet(BYTE* packet, bool ETHERNET_FRAME, DWORD size) :raw_pkt((u_char*)packet) {

        create(ETHERNET_FRAME);
    }

    Packet(u_char* pkt, bool ETHERNET_FRAME = true) :raw_pkt(pkt) {

        create(ETHERNET_FRAME);
    }

    void create(bool ETHERNET_FRAME = true);
    void printIP();

    int ETHERNET_HEADER_SIZE;
    u_char* raw_pkt;
    IP_header* ip_header;
    TCP_header* tcp_header;
    UDP_header* udp_header;
    uint16_t packet_length;
    uint16_t ip_header_length;
    uint16_t trans_header_length;
    uint16_t payload_length;
    u_char* payload;
    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];

};



