#pragma once

#include "types.hpp"
#include "vector.hpp"

/**
 * This file defines the interface between 
 * the platform layer and the game logic
 */
namespace Game {

	// Forward declaration of Game::Data (Platform layer shouldn't care about contents)
	struct Data;

	// Filled out by platform layer every frame
	struct Input {
		Vec2Int frame_buffer_size;
		
		float64 delta_time;
		float64 frame_time;
		
		bool left_key_pressed;
		bool right_key_pressed;
	};

	// Initialize the game
	Data* init(const Input* input);

	// Update the game logic for a frame
	void update(const Input* input, Data* state);

	// Render a frame
	void render(const Input* input, Data* state);
}