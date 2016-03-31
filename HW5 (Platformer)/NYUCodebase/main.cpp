#include "Game.h"

int main(int argc, char *argv[]){
	Game myGame;
	while (!myGame.UpdateAndRender()) {} //you can press 0 to exit the game
	return 0;
}
