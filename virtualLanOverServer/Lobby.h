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

	}

	
	Server* server;
	vector<Client> memberList;
	string myip;

	bool join = false;

	void joinlLobby() {
		server->sendData("command join " + myip);
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
				cout << "joined\n";
			}else if(tokens[2] == "0"){}
		}		
		if (tokens[1] == "list") {
			if (tokens[2] == "1") {

				updateLobbyList(tokens);
				cout << "list updated\n";

			}else if(tokens[2] == "0"){
				join = false;
			}
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

