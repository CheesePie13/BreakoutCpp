#include "game.hpp"
#include <iostream>

struct GameState {
	float ballPosX;
	float ballPosY;
};

GameState* game_init(GameInput* gameInput) {
	std::cout << "Game Init" << std::endl;
	GameState* gameState = (GameState*) malloc(sizeof(GameState));
	gameState->ballPosX = 1;
	gameState->ballPosY = 3;
	return gameState;
}

void game_update(GameInput* gameInput, GameState* gameState) {
	std::cout << "Game Update" << std::endl;
	gameState->ballPosX += 1;
	gameState->ballPosY -= 1;
}

void game_render(GameInput* gameInput, GameState* gameState) {
	std::cout << "Game Render" << std::endl;
	std::cout << "Ball Pos: " << gameState->ballPosX << " " << gameState->ballPosY << std::endl;
}