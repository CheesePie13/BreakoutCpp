#include <iostream>
#include <math.h>

#include "game.hpp"
#include "shader.hpp"
#include "fileloader.hpp"
#include "vector.hpp"
#include "collision.hpp"

using namespace Game;

const float32 PI = 3.14159265;

const Vec2 world_size = {16, 9};

const int     tile_grid_size_x   = 12;
const int     tile_grid_size_y   = 3;
const Vec2Int tile_grid_size     = {tile_grid_size_x, tile_grid_size_y};
const int32   tile_count         = tile_grid_size_x * tile_grid_size_y;

const Vec2    tile_size          = {1.0f, 0.5f};
const Vec2    tile_grid_offset   = {0.0f, -1.0f};

const float32 paddle_start_pos_x = 0.0f;
const float32 paddle_pos_y       = -4.0f;
const Vec2    paddle_size        = {2.0f, 0.25f};
const float32 paddle_speed       = 6.0f;

const Vec2    ball_start_pos     = {0.0f, 0.0f};
const float32 ball_radius        = 0.2f;
const float32 ball_base_speed    = 4.5f; // Ball speed on level 1
const float32 ball_level_speed   = 0.5f; // Speed the ball increases by every level

const float32 ball_paddle_max_rotation = 15.0f * (3.14159f / 180.0f);

enum GameState {
	PAUSED,
	PLAYING, 
	GAME_OVER
};

struct Tile {
	Vec2 pos;
	int32 health;
};

struct Game::Data {
	GameState state;
	int32     score;
	int32     level;
	int32     lives;

	float32 paddle_pos_x;

	Vec2 ball_pos;
	Vec2 ball_vel;
	Tile tiles[tile_count];
	
	uint32 rectangle_shader;
	uint32 circle_shader;
	uint32 quad_vao;
};

void reset_ball_and_paddle(Data* data, float32 ball_speed) {
	data->ball_pos = ball_start_pos;
	float32 angle = (rand() / (float32)RAND_MAX) * (PI * 0.5f) + (PI * 0.25f); // From 45 to 135 degrees
	data->ball_vel.x = ball_speed * cos(angle);
	data->ball_vel.y = ball_speed * sin(angle);

	data->paddle_pos_x = paddle_start_pos_x;
}

void reset_game(Data* data) {
	data->score = 0;
	data->level = 1;
	data->lives = 2;
	reset_ball_and_paddle(data, ball_base_speed);
	for (int i = 0; i < tile_count; i++) {
		data->tiles[i].health = 1;
	}
}

Data* Game::init(const Input* input) {
	
	//
	// Initialize game data
	//
	Data* data = new Data;

	//
	// OpenGL set up
	//
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//
	// Set up rectangle shader
	//
	char* rectangle_vert_shader = load_file("shaders/rectangle.vs");
	char* rectangle_frag_shader = load_file("shaders/rectangle.fs");
	if (rectangle_vert_shader == NULL || rectangle_frag_shader == NULL) {
		return NULL;
	}

	data->rectangle_shader = create_shader_program(rectangle_vert_shader, rectangle_frag_shader);

	free_file(rectangle_vert_shader);
	free_file(rectangle_frag_shader);

	if (data->rectangle_shader == 0) {
		std::cout << "Failed to create rect shader program." << std::endl;
		return NULL;
	}

	//
	// Set up circle shader
	//
	char* circle_vert_shader = load_file("shaders/circle.vs");
	char* circle_frag_shader = load_file("shaders/circle.fs");
	if (circle_vert_shader == NULL || circle_frag_shader == NULL) {
		return NULL;
	}

	data->circle_shader = create_shader_program(circle_vert_shader, circle_frag_shader);

	free_file(circle_vert_shader);
	free_file(circle_frag_shader);

	if (data->circle_shader == 0) {
		std::cout << "Failed to create circle shader program." << std::endl;
		return NULL;
	}

	//
	// Set up quad vao
	//
	float32 verticies[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	uint32 indices[] = {
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	glGenVertexArrays(1, &data->quad_vao);
	glBindVertexArray(data->quad_vao);

	uint32 index_buffer;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	uint32 vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float32) * 3, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//
	// Set up tile positions
	//
	{
		// Start at upper left tile
		Vec2 cur_pos;
		cur_pos.y = (world_size.y * 0.5f) - (tile_size.y * 0.5f) + tile_grid_offset.y;
		for (int y = 0; y < tile_grid_size.y; y++) {
			cur_pos.x = -(tile_size.x * tile_grid_size.x * 0.5f) + (tile_size.x * 0.5f) + tile_grid_offset.x;
			for (int x = 0; x < tile_grid_size.x; x++) {
				Tile* tile = &data->tiles[x + y * tile_grid_size.x];
				tile->pos = cur_pos;
				cur_pos.x += tile_size.x;
			}
			cur_pos.y -= tile_size.y;
		}
	}

	//
	// Reset Game
	//
	reset_game(data);
	data->state = PAUSED;

	std::cout << "Game Initialized" << std::endl;
	return data;
}

void Game::update(const Input* input, Data* data) {

	if (data->state == PAUSED) {
		if (input->start_key_pressed && !input->start_key_pressed_prev) {
			data->state = PLAYING;
		} else {
			return;
		}
	} else if (data->state == GAME_OVER) {
		if (input->start_key_pressed && !input->start_key_pressed_prev) {
			reset_game(data);
			data->state = PLAYING;
		} else {
			return;
		}
	} else if (data->state == PLAYING) {
		if (input->start_key_pressed && !input->start_key_pressed_prev) {
			data->state = PAUSED;
			return;
		}
	}

	// Paddle Movement
	{
		int32 direction = 0;

		if (input->left_key_pressed) {
			direction -= 1;
		}

		if (input->right_key_pressed) {
			direction += 1;
		}

		Vec2 delta = (Vec2){direction * paddle_speed * (float32)input->delta_time, 0.0f};

		// Prevent movement if we would collide with the ball
		// @refactor: The check moves the ball in the opposite direction that the paddle is moving
		// to check for the collision. Not ideal but works for now.
		float32 distance;
		Vec2 point, normal;
		const Vec2 paddle_pos = (Vec2){data->paddle_pos_x, paddle_pos_y};
		if (!moving_circle_to_retangle_collision_check(data->ball_pos, data->ball_pos - delta, ball_radius, paddle_pos, paddle_size, 
				&distance, &point, &normal))
		{
			data->paddle_pos_x += delta.x;
		}
		
		float32 max_pos_x = (world_size.x * 0.5f);
		if (data->paddle_pos_x > max_pos_x) {
			data->paddle_pos_x = max_pos_x;
		} else if (data->paddle_pos_x < -max_pos_x) {
			data->paddle_pos_x = -max_pos_x;
		}
	}

	// Ball Movement And Collision
	// @cleanup: The logic here for the collision checking could be cleaner
	{
		Vec2 old_ball_pos = data->ball_pos;
		Vec2 new_ball_pos = data->ball_pos + (data->ball_vel * input->delta_time);

		Vec2 delta = new_ball_pos - old_ball_pos;
		float32 remaining_distance = magnitude(delta);

		int iteration = 0;
		while (remaining_distance > 0) {
			iteration++;
			if (iteration >= 5) {
				std::cout << "Warning: Hit max collision iterations." << std::endl;
				break;
			}

			float32 closest_distance = INFINITY;
			Vec2 closest_point, closest_normal;

			float32 distance;
			Vec2 point, normal;

			// Right wall collision
			if (moving_circle_to_vertical_line_collision_check(old_ball_pos, new_ball_pos, ball_radius, world_size.x * 0.5f, 
					&distance, &point, &normal) && distance < closest_distance)
			{
				closest_distance = distance;
				closest_point = point;
				closest_normal = normal;
			}

			// Left wall collision
			if (moving_circle_to_vertical_line_collision_check(old_ball_pos, new_ball_pos, ball_radius, -world_size.x * 0.5f, 
					&distance, &point, &normal) && distance < closest_distance)
			{
				closest_distance = distance;
				closest_point = point;
				closest_normal = normal;
			}

			// Top wall collision
			if (moving_circle_to_horizontal_line_collision_check(old_ball_pos, new_ball_pos, ball_radius, world_size.y * 0.5f, 
					&distance, &point, &normal) && distance < closest_distance)
			{
				closest_distance = distance;
				closest_point = point;
				closest_normal = normal;
			}

			// Paddle collision
			bool hit_paddle = false;
			const Vec2 paddle_pos = (Vec2){data->paddle_pos_x, paddle_pos_y};
			if (moving_circle_to_retangle_collision_check(old_ball_pos, new_ball_pos, ball_radius, paddle_pos, paddle_size, 
					&distance, &point, &normal) && distance < closest_distance) 
			{
				closest_distance = distance;
				closest_point = point;
				closest_normal = normal;
				hit_paddle = true;
			}

			// Tile collision
			Tile* hit_tile = NULL;
			for (int i = 0; i < tile_count; i++) {
				Tile* tile = &data->tiles[i];
				if (tile->health < 1) {
					continue;
				}

				if (moving_circle_to_retangle_collision_check(old_ball_pos, new_ball_pos, ball_radius, tile->pos, tile_size, 
					&distance, &point, &normal) && distance < closest_distance) 
				{
					closest_distance = distance;
					closest_point = point;
					closest_normal = normal;
					hit_tile = tile;
					hit_paddle = false;
				}
			}

			// Check closest collision
			if (closest_distance < remaining_distance) {
				remaining_distance -= closest_distance;
				data->ball_vel = reflect(data->ball_vel, closest_normal);
				
				// Add extra rotation if hitting the paddle based on how far the hit was from the center
				if (hit_paddle && closest_normal.y > 0.99f) {
					float32 rotation = ((paddle_pos.x - point.x) / paddle_size.x) * 2.0f * ball_paddle_max_rotation;
					data->ball_vel = rotate(data->ball_vel, rotation);
				}

				if (hit_tile != NULL) {
					hit_tile->health--;
					data->score++;
				}
				
				old_ball_pos = closest_point;
				new_ball_pos = closest_point + normalize(data->ball_vel) * remaining_distance;
				delta = new_ball_pos - old_ball_pos;
			} else {
				remaining_distance = 0.0f;
			}
		}

		// Update ball pos
		data->ball_pos = new_ball_pos;
	}

	// Check Advance Level
	{
		bool has_health = false;
		for (int i = 0; i < tile_count; i++) {
			if (data->tiles[i].health > 0) {
				has_health = true;
				break;
			}
		}

		if (!has_health) {
			data->level++;
			reset_ball_and_paddle(data, ball_base_speed + ball_level_speed * (data->level - 1));
			for (int i = 0; i < tile_count; i++) {
				data->tiles[i].health = data->level;
			}
			
			data->state = PAUSED;
		}
	}

	// Check Game Over
	{
		if (data->ball_pos.y < -world_size.y * 0.5f) {
			if (data->lives > 0) {
				data->lives--;
				reset_ball_and_paddle(data, ball_base_speed + ball_level_speed * (data->level - 1));
				data->state = PAUSED;
			} else {
				data->state = GAME_OVER;
			}
		}
	}
}

void Game::render(const Input* input, Data* data) {

	// Update window title
	switch (data->state) {
		case PAUSED:
			input->update_ui(data->score, data->lives, "Press Spacebar to Play");
			break;
		case PLAYING:
			input->update_ui(data->score, data->lives, NULL);
			break;
		case GAME_OVER:
			input->update_ui(data->score, data->lives, "Press Spacebar to Play Again");
			break;
	}

	const float world_to_clip[9] = { 
		2.0f / world_size.x, 0.0f, 0.0f,
		0.0f, 2.0f / world_size.y, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	glViewport(0, 0, input->frame_buffer_size.x, input->frame_buffer_size.y);

	glClearColor(0.0f, 0.5f, 0.5f, 7.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//
	// Render the quads
	//
	glBindVertexArray(data->quad_vao);

	// Circles
	{
		glUseProgram(data->circle_shader);

		int center_pos_location    = glGetUniformLocation(data->circle_shader, "center_pos");
		int radius_location        = glGetUniformLocation(data->circle_shader, "radius");
		int world_to_clip_location = glGetUniformLocation(data->circle_shader, "world_to_clip");

		glUniformMatrix3fv(world_to_clip_location, 1, GL_FALSE, world_to_clip);

		// Render ball
		glUniform2f(center_pos_location, data->ball_pos.x, data->ball_pos.y);
		glUniform1f(radius_location, ball_radius);
		glUniformMatrix3fv(world_to_clip_location, 1, GL_FALSE, world_to_clip);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// Rectangles
	{
		glUseProgram(data->rectangle_shader);

		int center_pos_location    = glGetUniformLocation(data->rectangle_shader, "center_pos");
		int scale_location         = glGetUniformLocation(data->rectangle_shader, "scale");
		int world_to_clip_location = glGetUniformLocation(data->rectangle_shader, "world_to_clip");
		int alpha_location         = glGetUniformLocation(data->rectangle_shader, "alpha");

		glUniformMatrix3fv(world_to_clip_location, 1, GL_FALSE, world_to_clip);
		
		// Render paddle
		{
			glUniform2f(scale_location, paddle_size.x, paddle_size.y);
			glUniform2f(center_pos_location, data->paddle_pos_x, paddle_pos_y);
			glUniform1f(alpha_location, 1.0f);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		// Render tiles
		{
			// @optimize: These could be batched together
			glUniform2f(scale_location, tile_size.x - 0.05f, tile_size.y - 0.05f);
			for (int i = 0; i < tile_count; i++) {
				Tile tile = data->tiles[i];
				if (tile.health < 1) {
					continue;
				}

				glUniform2f(center_pos_location, tile.pos.x, tile.pos.y);
				glUniform1f(alpha_location, (float32)tile.health / (float32)data->level);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
		}
	}

	// @cleanup Find a better way to report errors instead of just at the end of rendering
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL Error: " << error << std::endl;
	}
}