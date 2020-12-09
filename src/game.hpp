#pragma once

/**
 * This file defines the interface between 
 * the platform layer and the game logic
 */

struct GameState;

struct GameInput {
	double deltaTime;
	bool leftKeyPressed;
	bool rightKeyPressed;
};

GameState* game_init(GameInput* gameInput);
void game_update(GameInput* gameInput, GameState* gameState);
void game_render(GameInput* gameInput, GameState* gameState);
