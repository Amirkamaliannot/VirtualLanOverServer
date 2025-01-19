#pragma once
#include <vector>
#include <iostream>
#include "Server.h"


struct client {
	std::string ip;
};


class Lobby
{
public:

	Lobby(Server* server, std::string myip) : server(server), myip(myip) {

	}

	
	Server* server;
	std::vector<client> memberList;
	std::string myip;

	bool join = false;

	void joinlLobby() {
		server->sendData("command join " + myip);
		join = true;
	}

	void getLobbyMembers() {
	
		server->sendData("command getList " + myip);
	};


	void updateLobbyList() {

	}

	void leaveLobby() {
		server->sendData("command leave " + myip);
		join = false;
	}	
	
	void sendData(std::string data) {
		server->sendData(data);
	}




};

