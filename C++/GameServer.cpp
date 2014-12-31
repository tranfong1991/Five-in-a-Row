#include "GameServer.h"
#include <algorithm>

char intToChar(int i){
	if(i<10)
			return i + 48;
		
	switch(i){
	case 10:
		return 'a';
	case 11: 
		return 'b';
	case 12:
		return 'c';
	case 13:
		return 'd';
	case 14:
		return 'e';
	default:
		return 'f';
	}
}

int charToInt(char c){
	if(c == 'a' || c== 'b' || c== 'c' || c=='d' || c == 'e' || c== 'f')
		return c -'a' + 10;
	if(c == 'A' || c== 'B' || c== 'C' || c=='D' || c == 'E' || c== 'F')
		return c -'A' + 10;
	return c - '0';
}

// if character is a hexadecimal character
bool isHex(char c) {
	if (isdigit(c)) return true;
	if (c <= 'f' && c >= 'a') return true;
	if (c <= 'F' && c >= 'A') return true;
	return false;
}

// if s matches move syntax
bool isMove(string s) {
	// 1 or xy is not a valid move
	if(s.length() < 2 || !isHex(s[0]) || !isHex(s[1])) return false;
	// abc is not a valid move
	if(s.length() > 2 && isalnum(s[2])) return false;
	return true;
}

// case insensitive version of string find
int insensitiveFind(const string& left, const string& right) {
	string left_lower = left;
	string right_lower = right;
	transform(left_lower.begin(), left_lower.end(), left_lower.begin(), ::tolower);
	transform(right_lower.begin(), right_lower.end(), right_lower.begin(), ::tolower);
	//cout << '[' << left_lower << ']' << ' ' << '[' << right_lower << ']' << endl;
	return left_lower.find(right_lower);
}

// Remove line breaks
string remBreak(string s) {
	string result = "";
	for(int i = 0; i < s.length(); ++i) {
		// the character w/ value 13 causes... issues
		if((int)(s[i]) != 13 && s[i] != '\n')
		result += s[i];
	}
	return result;
}

// Clean input (only readable characters)
string clean(string s) {
	string result = "";
	for(int i = 0; i < s.length(); ++i) {
		// the character w/ value 13 causes... issues
		if(((int)(s[i]) != 13 && s[i] != '\n') && (isalnum(s[i]) || isspace(s[i])) || s[i] == '-')
		result += s[i];
		else if (s[i] == ';') break;
	}
	return result;
}

/*
	Initializes the port number
	Sets the player stack and AI stack to 7, meaning that they can only undo 7 moves
	Sets the lock to the board until the correct password is received
	Passes the pointer of the board to the game logic
	Setup the server, making it ready for incoming connection
*/
GameServer::GameServer(int p): portNumber(p), aiStack(7), playerStack(7){
	for(int i = 0; i<15; i++){
		for(int j = 0; j<15; j++){
			board[i][j] = '+';
		}
	}
	
	unlocked = false;
	display = false;
	
	ai.set_board(&board);
	gameLogic.setBoard(&board);
	setupServer();
}

GameServer::~GameServer(){
	delete[] board;
}

void GameServer::setupServer(){
	struct sockaddr_in server;

	//Create socket
	clientSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	
	if(clientSocketDescriptor < 0){
		perror("FAILED TO CREATE SOCKET!");
		exit(1);
	}
	
	//Set socket information
	server.sin_family = AF_INET;
	server.sin_addr.s_addr=INADDR_ANY;
	server.sin_port=htons(portNumber);
	
	//Bind
	if(bind(clientSocketDescriptor, (sockaddr*)&server, sizeof(server))){
		perror("BIND FAILED!");
		exit(1);
	}
	
	//Listen. Only 1 connection can be queued
	listen(clientSocketDescriptor, 1);
}

/*
	Accepts a connection from other server/client
	Continuously receives message from the server/client and execute that command accordingly
*/
void GameServer::start(){
	cout<<"Waiting for connection...\n";

	struct sockaddr_storage clientInfo;
	socklen_t len;
	clientSocket = accept(clientSocketDescriptor, (struct sockaddr*) &clientInfo, &len);
	
	cout<<"Connection established.\n";
	sendMessage("PASSWORD\n\0");
	
	while(true){
		receiveMessage(clientSocket);	
		executeHumanAI();
	}
}

/*
	Connects to another server for AI vs. AI
	Takes care of all the error situations (i.e. incorrect host name, connection failure, socket creation failure)
*/
int GameServer::connectTo(const char* server, const char* port){
	int portNo = atoi(port);
	struct sockaddr_in serv_addr;
	struct hostent *serv;
	
	//Create socket
	serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocketDescriptor < 0){
		perror("FAIL TO CREATE SOCKET!");
		exit(1);
	}
	serv = gethostbyname(server);
	if(serv == NULL){
		sendMessage("; NO SUCH HOST\n\0", clientSocket);
		return -1;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)serv->h_addr, (char *)&serv_addr.sin_addr.s_addr,serv->h_length);
	serv_addr.sin_port = htons(portNo);
	
	//Connect to the server
	if(connect(serverSocketDescriptor, (struct sockaddr*) &serv_addr, sizeof(serv_addr))<0){
		sendMessage("; ERROR\n\0", clientSocket);
		return -1;
	}
	return 1;
}

/*
	AI makes a move, sends that move to another server
	Receives the respond and execute accordingly
	Returns -1 in special situations (i.e. board filled, AI 1 won, AI 2 won)
	Returns 1 in normal situations
*/
int GameServer::executeAIAI(){
	ai.set_diff(AI_dif::_HARD);
	pair<int,int> ai_move = ai.get_move();
	ai_move = ai.get_move();
	// the ai is friendly to itself, so we put player = AI even though on the board, the AI is 'human'
	ai.put_move(ai_move.first, ai_move.second, player::_AI);
	
	//first our move
	char move[] = {intToChar(ai_move.first+1), intToChar(ai_move.second+1),'\n','\0'};
	sendMessage(move,serverSocketDescriptor);
	
	if(gameLogic.hasWon(ai_move.first, ai_move.second, 'O')){
		board[ai_move.first][ai_move.second] = 'O';
		char move[] = "xy ; We Won!\n";
		move[0] = intToChar(ai_move.first+1);
		move[1] = intToChar(ai_move.second+1);
		//sendMessage("; We Won!\n\0");
		if (display) sendBoard();
		sendMessage(move);
		sendMessage("EXIT\n\0",serverSocketDescriptor);
		return -1;
	}
	if (display) sendBoard();
	board[ai_move.first][ai_move.second] = 'O';
	sendMessage(move);
	
	receiveMessage(serverSocketDescriptor);
	string s = clean(string(readBuffer));
	
	while (insensitiveFind(s,"OK") == 0 || s.length() < 2) {
		receiveMessage(serverSocketDescriptor);
		s = clean(string(readBuffer));
	}
	if(insensitiveFind(s,"ILLEGAL")==0) {
		sendMessage("; Something went wrong... we sent an illegal move\n\0");
	} 
	else if(isMove(s)){
		int x = charToInt(s[0]) - 1;
		int y = charToInt(s[1]) - 1;
		
		if(gameLogic.isLegalMove(x, y, '@')){
			if(gameLogic.hasWon(x, y, '@')){
				char otherMove[] = "xy ; Other AI Won!\n";
				otherMove[0] = s[0];
				otherMove[1] = s[1];
				if (display) sendBoard();
				sendMessage(otherMove);
				board[x][y] = '@';
				//sendMessage("; Other AI Won!\n\0");
				sendMessage("EXIT\n\0",serverSocketDescriptor);
				return -1;
			}
			board[x][y] = '@';
			ai.put_move(x, y, player::_HUMAN);
			if (display) sendBoard();
			char otherMove[] = {s[0], s[1], '\n', '\0'};
			sendMessage(otherMove);
		} else {
			sendMessage("ILLEGAL\n\0",serverSocketDescriptor);
		}
	}
	
	return 1;
}

/*
	Execute command received from client/server
	Takes care of changing AI difficulties, making moves, passwords, undoing, etc.
*/
void GameServer::executeHumanAI(){
	string s = string(readBuffer);
	s = clean(s);
	if(s.length() < 2) return;
	
	if(!unlocked) {
		if(s == PASSWORD){
			unlocked = true;
			sendMessage("WELCOME\n\0");
		}
		else
			sendMessage("; Incorrect Password!\n\0");
	}
	else if(insensitiveFind(s,"EXIT") == 0){
		sendMessage("OK\n\0");
		sendMessage("; Connection closed\n\0");
		close(clientSocket);
		close(clientSocketDescriptor);
		close(serverSocketDescriptor);
	}
	else if(insensitiveFind(s,"RESET") == 0){
		sendMessage("OK\n\0");
		resetBoard();
		aiStack.clear();
		playerStack.clear();
	}
	else if(insensitiveFind(s,"EASY")==0){
		sendMessage("OK\n\0");
		ai.set_diff(AI_dif::_EASY);
		resetBoard();
		aiStack.clear();
		playerStack.clear();
	}
	else if(insensitiveFind(s,"MEDIUM")==0){
		sendMessage("OK\n\0");
		ai.set_diff(AI_dif::_MEDIUM);
		resetBoard();
		aiStack.clear();
		playerStack.clear();
	}
	else if(insensitiveFind(s,"HARD")==0){
		sendMessage("OK\n\0");
		ai.set_diff(AI_dif::_HARD);
		resetBoard();
		aiStack.clear();
		playerStack.clear();
	}
	else if(insensitiveFind(s,"HUMAN-AI") ==0){
		sendMessage("OK\n\0");
	}
	else if(insensitiveFind(s,"AI-AI") ==0){
		resetBoard();
		ai.set_diff(AI_dif::_HARD);
		
		int index = 0;
		int status;
		string arguments = s.substr(6);
		string address = "", port = "", password = "";
		
		//Extracts host name / IP, port number, and password
		while(arguments[index] != ' ') 
			{ address += arguments[index]; ++index; }
		++index;
		while(arguments[index] != ' ') 
			{ port += arguments[index]; ++index; }
		++index;
		while(index != arguments.length()) 
			{ password += arguments[index]; ++index; }
		
		//Connects to the other AI server
		if(connectTo(address.c_str(), port.c_str())<0)
			return;
		receiveMessage(serverSocketDescriptor);
		string respond = string(readBuffer);
		sendMessage(password, serverSocketDescriptor);
		
		// eat password token if we get it
		if(insensitiveFind(respond,"PASSWORD") == 0){
			receiveMessage(serverSocketDescriptor);
			respond = string(readBuffer);
		}
		// if we don't get welcome message, entering password failed
		if(insensitiveFind(respond,"WELCOME") != 0){
			sendMessage("; Incorrect Password!\n\0");
			return;
		}
		sendMessage("HARD", serverSocketDescriptor);
		receiveMessage(serverSocketDescriptor);
		respond = string(readBuffer);
		if(insensitiveFind(respond,"OK") != 0){
			sendMessage("; Something went wrong!\n\0");
			return;
		}
		
		//Starts playing
		do{
			status = executeAIAI();
		}
		while(status > 0);
	}
	else if(insensitiveFind(s,"DISPLAY")==0) {
		sendMessage("OK\n\0");
		display = !display;
	}
	else if(insensitiveFind(s,"UNDO")==0){
		sendMessage("OK\n\0");
		if(playerStack.getSize() == 0){
			sendMessage("; No more moves\n\0");
			return;
		}
		Position playerPos = playerStack.pop();
		int x1 = charToInt(playerPos.getX()) - 1;
		int y1 = charToInt(playerPos.getY()) - 1;
		
		Position aiPos = aiStack.pop();
		int x2 = charToInt(aiPos.getX()) - 1;
		int y2 = charToInt(aiPos.getY()) - 1;
		
		board[x1][y1] = '+';
		board[x2][y2] = '+';
		
		char playerUndo[] = {';', ' ', 'U', 'N', 'D', 'O', ' ', playerPos.getX(), playerPos.getY(), '\n', '\0'};
		char aiUndo[] = {';', ' ', 'U', 'N', 'D', 'O', ' ', aiPos.getX(), aiPos.getY(), '\n', '\0'};
		sendMessage(playerUndo);
		sendMessage(aiUndo);
		
		ai.set_board(&board);
		printBoard();
		if(display) sendBoard();
	}
	else if(isMove(s)){
		sendMessage("OK\n\0");
		int x = charToInt(s[0]) - 1;
		int y = charToInt(s[1]) - 1;
		
		if(gameLogic.isLegalMove(x, y, 'O')){
			playerStack.push(Position(s[0], s[1]));
			
			if(gameLogic.hasWon(x, y, 'O')){
				board[x][y] = 'O';
				sendMessage("; You Won!\n\0");
				printBoard();
				if(display) sendBoard();
				return;
			}
			board[x][y] = 'O';
			
			// tell ai that human has moved in x, y
			ai.put_move(x, y, player::_HUMAN);
			bool valid_ai_move = false;
			pair<int,int> ai_move = ai.get_move();
			// while looking for a legal AI move
			while (!gameLogic.isLegalMove(ai_move.first, ai_move.second, '@')) {
				ai_move = ai.get_move();
			}
			aiStack.push(Position(intToChar(ai_move.first+1), intToChar(ai_move.second+1)));
			
			if(gameLogic.hasWon(ai_move.first, ai_move.second, '@')){
				board[ai_move.first][ai_move.second] = '@';
				char move[] = {intToChar(ai_move.first+1), intToChar(ai_move.second+1),' ',';',' ','A','I',' ','W','o','n','!','\n','\0'};
				sendMessage(move);
				printBoard();
				if(display) sendBoard();
				return;
			}
			board[ai_move.first][ai_move.second] = '@';
			// tell ai that the ai has moved
			ai.put_move(ai_move.first, ai_move.second, player::_AI);
			
			printBoard();
			if(display) sendBoard();
			
			char move[] = {intToChar(ai_move.first+1), intToChar(ai_move.second+1),'\n','\0'};
			sendMessage(move);
		} else {
			sendMessage("ILLEGAL\n\0");
		}
	}
	else {
		sendMessage("; Could not parse command!\n\0");
	}
}

//Receives message from client/server with the corresponding socket descriptor and puts that in the readBuffer
int GameServer::receiveMessage(int socket){
	memset(readBuffer, 0, BUFFER_SIZE);
	int val = recv(socket, readBuffer, BUFFER_SIZE, 0);
	cout<<">> "<<remBreak(readBuffer) << endl;
	
	if(val<0){
		close(socket);
		exit(1);
	}
	
	return val;
}

//Sends message to the client
int GameServer::sendMessage(string message){
	sendMessage(message,clientSocket);
}

//Sends message to the corresponding socket descriptor
int GameServer::sendMessage(string message, int socket){
	char m[message.length()];
	strcpy(m, message.c_str());
	
	size_t sent = write(socket, m, strlen(m));
	cout<<"<< "<< remBreak(message) << endl;
	
	if(sent < 0){
		close(socket);
		exit(1);
	}
	return sent;
}

void GameServer::resetBoard(){
	for(int i = 0; i<15; i++){
		for(int j = 0; j<15; j++){
			board[i][j] = '+';
		}
	}
	ai.set_board(&board);
}

void GameServer::printBoard(){
	cout << "  1 2 3 4 5 6 7 8 9 a b c d e f" << endl;
	for(int i = 0; i<15; i++){
		cout << hex << (i+1) << ' ';
		for(int j = 0; j<15; j++){
			cout<<board[i][j]<<" ";
		}
		cout<<endl;
	}
	cout << dec;
}

void GameServer::sendBoard(){
	sendMessage(";  1 2 3 4 5 6 7 8 9 a b c d e f\n\0");
	for(int i = 0; i<15; i++){
		char thisrow[] = ";x + + + + + + + + + + + + + + +\n\0"; 
		thisrow[1] = intToChar(i+1);
		for(int j = 0; j<15; j++){
			thisrow[1+2*(j+1)] = board[i][j];
		}
		sendMessage(thisrow);
	}
}

