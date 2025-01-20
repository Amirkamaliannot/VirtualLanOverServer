#pragma once
#include <vector>
#include <iostream>
#include "Server.h"
#include <string>

using namespace std;

struct Client {
	std::string ip;
};


class Lobby
{
public:

	Lobby(Server* server, std::string myip) : server(server), myip(myip) {

		joinlLobby();
	}


	
	Server* server;
	vector<Client> memberList;
	string myip;

	bool join = false;

	void startCheckConnection() {
		std::thread listen(&Lobby::checkConnection, this);
		listen.detach(); // Detach the thread if you don't want to join it later
	}

	void checkConnection() {
		while (!server->END)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10000));
			if (!server->connected) {
				memberList.clear();
				join = false;
				server->reconnect();
			}
			else if (server->connected && !join) {
				joinlLobby();
			}
			else {

				getLobbyMembers();
			}
		}
	}

	void joinlLobby() {
		server->sendData("command join " + myip);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		getLobbyMembers();
	}

	void getLobbyMembers() {
	
		server->sendData("command list " + myip);
	};


	void updateLobbyList(vector<std::string> tokens) {
		memberList.clear();
		for (int i = 2; i < tokens.size(); i++) {
			Client client{tokens[i]};
			memberList.push_back(client);
		}
	}

	void showMemberList() {
		for (int i = 0; i < memberList.size(); i++) {
			cout << memberList[i].ip << "\n";
		}
	}

	void leaveLobby() {
		server->sendData("command leave " + myip);
		join = false;
	}	
	
	void sendData(std::string data) {
		server->sendData(data);
	}


	void reconnect() {
		server->reconnect();
	}


	void handleResponse(BYTE* command, DWORD size) {

		vector<std::string> tokens;
		string str(reinterpret_cast<const char*>(command), size);
		string temp = "";
		std::string delimiter = " ";
		size_t pos = 0;
		std::string token;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			tokens.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		tokens.push_back(str);

		if (tokens[1] == "join") {

			if (tokens[2] == "1") {
				join = true;

			}else if(tokens[2] == "0"){}
		}		
		if (tokens[1] == "list") {
			if (tokens[2] == "1") {

				updateLobbyList(tokens);

			}else if(tokens[2] == "0"){}
		}

		if (tokens[1] == "leave") {
			if (tokens[2] == "1") {
				memberList.clear();
				join = false;
			}
			else if (tokens[2] == "0") {}
		}		
		
		if (tokens[1] == "error") {
			join = false;
		}
		


	}



};

