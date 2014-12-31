#pragma once
#include "GameServer.h"

int main(int argc, char* argv[]){
	GameServer server(atoi(argv[1]));
	server.start();

    return 0; 
}