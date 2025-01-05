#pragma once
#include <steam/steam_api.h>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

class SteamP2PConnection {
private:
    CSteamID remotePeerID;
    bool isConnected;
    int connectionTimeout;
    std::vector<EP2PSend> sendTypes;

public:
    SteamP2PConnection() : isConnected(false), connectionTimeout(5000) {
        // انواع مختلف ارسال را اضافه می‌کنیم
        sendTypes = {
            k_EP2PSendReliable,           // برای داده‌های مهم
            k_EP2PSendUnreliable,         // برای داده‌های کم اهمیت
            k_EP2PSendReliableWithBuffering // برای داده‌های بزرگ
        };
    }

    bool Connect(CSteamID peerID) {
        remotePeerID = peerID;

        // تلاش برای اتصال با روش‌های مختلف
        for (auto sendType : sendTypes) {
            if (TryConnection(sendType)) {
                isConnected = true;
                return true;
            }
        }

        return false;
    }

private:
    bool TryConnection(EP2PSend sendType) {
        // ارسال بسته handshake
        const char* data = "HANDSHAKE";
        if (!SteamNetworking()->SendP2PPacket(remotePeerID, data, strlen(data) + 1, sendType)) {
            return false;
        }

        // منتظر پاسخ می‌مانیم
        auto startTime = std::chrono::steady_clock::now();

        while (true) {
            uint32 msgSize;
            if (SteamNetworking()->IsP2PPacketAvailable(&msgSize)) {
                std::vector<char> buffer(msgSize);
                CSteamID sender;

                if (SteamNetworking()->ReadP2PPacket(buffer.data(), msgSize, &msgSize, &sender)) {
                    if (sender == remotePeerID) {
                        return true;
                    }
                }
            }

            // بررسی timeout
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>
                (currentTime - startTime).count();

            if (elapsedTime > connectionTimeout) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return false;
    }

public:
    bool SendData(const void* data, int size) {
        if (!isConnected) return false;

        // تلاش برای ارسال با روش‌های مختلف
        for (auto sendType : sendTypes) {
            if (SteamNetworking()->SendP2PPacket(remotePeerID, data, size, sendType)) {
                return true;
            }

            // کمی صبر می‌کنیم قبل از تلاش مجدد
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return false;
    }

    bool ReceiveData(std::vector<char>& outData) {
        if (!isConnected) return false;

        uint32 msgSize;
        if (!SteamNetworking()->IsP2PPacketAvailable(&msgSize)) {
            return false;
        }

        outData.resize(msgSize);
        CSteamID sender;

        if (SteamNetworking()->ReadP2PPacket(outData.data(), msgSize, &msgSize, &sender)) {
            return (sender == remotePeerID);
        }

        return false;
    }

    void Disconnect() {
        if (isConnected) {
            SteamNetworking()->CloseP2PSessionWithUser(remotePeerID);
            isConnected = false;
        }
    }

    bool IsConnected() const {
        return isConnected;
    }
};