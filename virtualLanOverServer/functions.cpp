#include "functions.h"
#include <string> 
#include <fstream> 
#include <iostream>
#include <string>
#include <sstream> 
#include <zlib.h> 

std::wstring systemIP = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(getIP());
WintunManager wintunManager(L"AmirVPN", systemIP, L"255.0.0.0");
Server server(readingIp());
Lobby lobby(&server, getIP());



void callbackLitentToInterface(BYTE* packet, DWORD size)
{
    Packet temp = Packet(packet, false, size);

    for (int i = 0; i < lobby.memberList.size(); i++) {

        if (lobby.join && strcmp(temp.dst_ip, lobby.memberList[i].ip.c_str() ) == 0) {
            
            auto start = std::chrono::high_resolution_clock::now();
            Data compress = compressZlib(packet, size);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << "Time taken by function: " << duration.count() << " microseconds\n";
            server.sendData(compress.data, compress.size);
            delete[] compress.data;
        }
    }
}

void callbackLiteningToServer(BYTE* packet, DWORD size)
{
    if (startsWithCommand(packet, size)) {

        lobby.handleResponse(packet, size);

    }else{
        Data Decompress = DecompressZlib(packet, size);
        wintunManager.sendPacket(Decompress.data, Decompress.size);
        delete[] Decompress.data;
        std::cout << "miror\n";
    }
}

std::string readingIp()
{
    // Open the file
    std::ifstream file("server.txt");
    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
    }
    // Read the entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_content = buffer.str();
    // Close the file
    file.close();
    return file_content;
}

std::string GetVolumeSerialNumber()
{
    char volumePath[MAX_PATH] = "C:\\"; // Use the system drive
    DWORD serialNumber = 0;

    // Retrieve the volume serial number
    if (GetVolumeInformation(L"C:\\", nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0)) {
        char buffer[20];
        sprintf_s(buffer, "%08X", serialNumber); // Format as an 8-character hexadecimal string
        return std::string(buffer);
    }
    else {
        std::cerr << "Failed to retrieve volume serial number: " << GetLastError() << std::endl;
        return "";
    }
}

std::string getIP()
{
    std::string serial = GetVolumeSerialNumber();
    std::string ip = "10";
    for (int i = 0; i < 3; i++) {
        ip += ".";
        auto a = serial[i*2] + serial[i*2 + 1];
        if (a > 250)a = a % 250;
        ip += std::to_string(a);
    }
    return ip;
}

bool startsWithCommand(const BYTE* data, size_t dataSize)
{
    const char* commandPrefix = "command";
    size_t prefixLength = strlen(commandPrefix);
    if (dataSize < prefixLength) {
        return false;
    }
    return memcmp(data, commandPrefix, prefixLength) == 0;
}

Data compressLZ4(BYTE* input, DWORD inputSize)
{

    cout << "Unmpressed size :" << inputSize << " bytes\n";

    // Allocate memory for compressed data
    int maxCompressedSize = LZ4_compressBound(inputSize);
    BYTE* compressedData = new BYTE[maxCompressedSize];

    // Compress the data
    int compressedSize = LZ4_compress_HC(
        reinterpret_cast<const char*>(input),
        reinterpret_cast<char*>(compressedData),
        inputSize,
        maxCompressedSize,
        LZ4HC_CLEVEL_MAX
    );

    if (compressedSize > 0) { cout << "Compressed size: " << compressedSize << " bytes\n"; }
    return Data{ compressedData , compressedSize };

}

Data compressZlib(BYTE* input, DWORD inputSize) {
    std::cout << "Uncompressed size: " << inputSize << " bytes\n";

    // Calculate maximum compressed size
    uLong maxCompressedSize = compressBound(inputSize);

    // Allocate memory for compressed data
    BYTE* compressedData = new BYTE[maxCompressedSize];

    // Compress the data using zlib
    uLong compressedSize = maxCompressedSize;
    int compressResult = compress(
        compressedData, &compressedSize,               // Output buffer and size
        reinterpret_cast<const Bytef*>(input), inputSize // Input data
    );

    if (compressResult == Z_OK) {
        std::cout << "Compressed size: " << compressedSize << " bytes\n";
    }
    else {
        std::cerr << "Compression failed with error code: " << compressResult << std::endl;
        // Clean up and return an empty compressData object
        delete[] compressedData;
        return { nullptr, 0 };
    }

    // Return the compressed data and its size
    return Data{ compressedData, (int)compressedSize };
}

Data DecompressLZ4(BYTE* compressedData, int compressedSize)
{
    int inputSize = 1500; //size of MTU
    // Allocate memory for decompressed data
    BYTE* decompressedData = new BYTE[inputSize];
    int decompressedSize = LZ4_decompress_safe(
        reinterpret_cast<const char*>(compressedData),
        reinterpret_cast<char*>(decompressedData),
        compressedSize,
        inputSize);

    if (decompressedSize > 0) {
        //std::cout << "Decompression successful! Data: " << decompressedData << "\n";
    }
    else {
        std::cout << "Decompression failed.\n";
    }
    return Data{ compressedData , compressedSize };
}
Data DecompressZlib(BYTE* compressedData, int compressedSize) {

    int inputSize = 1500; //size of MTU
    // Allocate memory for decompressed data
    BYTE* decompressedData = new BYTE[inputSize];

    // Decompress the data using zlib
    uLong decompressedSize = inputSize;
    int decompressResult = uncompress(
        reinterpret_cast<Bytef*>(decompressedData), &decompressedSize, // Output buffer and size
        reinterpret_cast<const Bytef*>(compressedData), compressedSize // Compressed data
    );

    if (decompressResult == Z_OK) {
        //std::cout << "Decompression successful! Data: "<< "\n";
        //for (int i = 0; i < inputSize; i++) {
        //    cout << decompressedData[i];
        //}
        //cout << "\n";
    }
    else {
        std::cerr << "Decompression failed with error code: " << decompressResult << "\n";
    }

    return Data{ compressedData , compressedSize };
}

;
