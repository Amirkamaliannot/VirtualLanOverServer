#include "functions.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <vector>
using std::setw;

int main() {

    std::cout << "start listening on interface\n";
    wintunManager.startListening(callbackLitentToInterface);

    //std::cout << "start listening on steam\n";
    //steam.startListening(callbackLiteningToSteam);

    std::cout << "\n\n";
    std::cout << "1 - Get friends list\n";
    std::cout << "2 - Create and join a lobby\n";
    std::cout << "3 - Leave a lobby\n";
    std::cout << "4 - Search and join first lobby\n";
    std::cout << "5 - Enter a lobby (by address)\n";
    std::cout << "6 - Get your lobby members list\n";


    while (true) {

        std::cout << "\nEnter command: ";

        double a;
        std::cin >> a;

            
        if (a == 0) {
            break;
        }
    }
    return 1;
}

