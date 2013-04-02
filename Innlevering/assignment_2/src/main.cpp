#include "GameManager.h"
#include <iostream>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

/**
 * Simple program that starts our game manager
 */
int main(int argc, char *argv[]) {
	try {
		GameManager* game;
		game = new GameManager();
		game->init();
		game->play();
		delete game;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		return -1;
	}
	return 0;
}
