#pragma once

#include "types.h"

/**
 * This file defines the interface between 
 * the platform layer and the game logic
 */
namespace Game {

	// Forward declaration of Game::State (Platform layer should care about contents)
	struct State;

	// Filled out by platform layer every frame
	struct Input {
		int32 frame_buffer_width;
		int32 frame_buffer_height;
		
		float64 delta_time;
		float64 frame_time;
		
		bool left_key_pressed;
		bool right_key_pressed;
	};

	// Initialize the game
	State* init(const Input* input);

	// Update the game logic for a frame
	void update(const Input* input, State* state);

	// Render a frame
	void render(const Input* input, State* state);
}