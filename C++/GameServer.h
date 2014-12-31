#pragma once
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Position.h"
#include "CircularBoundedStack.h"
#include "GameLogic.h"
#include "AI.h"

#define BUFFER_SIZE 256

class GameServer{
	const string PASSWORD = "123";
	
	bool unlocked;
	bool display;
	int clientSocketDescriptor;
	int serverSocketDescriptor;
	int clientSocket;
	int portNumber;
	char readBuffer[BUFFER_SIZE];
	char board[15][15];
	CircularBoundedStack<Position> aiStack;
	CircularBoundedStack<Position> playerStack;
	GameLogic gameLogic;
	AI ai;
	
	void setupServer();
	int connectTo(const char* server, const char* port);
	void executeHumanAI();
	int executeAIAI();
	int receiveMessage(int socket);
	int sendMessage(string message);
	int sendMessage(string message, int socket);
public:
	GameServer(int p);
	~GameServer();

	void start();
	void resetBoard();
	void printBoard();
	void sendBoard();
};