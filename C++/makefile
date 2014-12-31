all: ServerMain

GameLogic.o: GameLogic.h GameLogic.cpp
					g++-4.7 -std=c++11 -c GameLogic.h GameLogic.cpp
					
AI_util.o: AI_util.h AI_util.cpp
					g++-4.7 -std=c++11 -c AI_util.h AI_util.cpp

AI.o: AI.h AI.cpp
					g++-4.7 -std=c++11 -c AI.h AI.cpp
					
GameServer.o: GameServer.h GameServer.cpp GameLogic.o AI.o CircularBoundedStack.h Position.h
					g++-4.7 -std=c++11 -c GameServer.h GameServer.cpp GameLogic.o AI.o CircularBoundedStack.h Position.h

ServerMain: ServerMain.cpp GameServer.o GameLogic.o AI.o AI_util.o CircularBoundedStack.h Position.h
					g++-4.7 -std=c++11 -o ServerMain ServerMain.cpp GameServer.o GameLogic.o AI.o AI_util.o CircularBoundedStack.h Position.h
					
clean:
					rm -rf *o ServerMain