#include <iostream>

#include "game.hpp"
#include "shader.hpp"
#include "fileloader.hpp"

using namespace Game;

const float32 world_width  = 16;
const float32 world_height = 9;
const float32 move_speed   = 5;

struct Game::State {
	float pos_x;
	float pos_y;
	float scale_x;
	float scale_y;
	
	uint32 quad_program;
	uint32 quad_vao;
};

State* Game::init(const Input* input) {
	//
	// Initialize game state
	//
	State* state = new State;
	state->pos_x   = 0;
	state->pos_y   = 0;
	state->scale_x = 1;
	state->scale_y = 1;

	//
	// Set up quad shader
	//
	char* quad_vert_shader = load_file("shaders/quad.vs");
	char* quad_frag_shader = load_file("shaders/quad.fs");
	if (quad_vert_shader == NULL || quad_frag_shader == NULL) {
		return NULL;
	}

	uint32 quad_program = create_shader_program(quad_vert_shader, quad_frag_shader);

	free_file(quad_vert_shader);
	free_file(quad_frag_shader);

	if (quad_program == 0) {
		std::cout << "Failed to create shader program." << std::endl;
		return NULL;
	}

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

	uint32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

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

	state->quad_program = quad_program;
	state->quad_vao = vao;

	std::cout << "Game Initialized" << std::endl;
	return state;
}

void Game::update(const Input* input, State* state) {
	int32 direction = 0;

	if (input->left_key_pressed) {
		direction -= 1;
	}

	if (input->right_key_pressed) {
		direction += 1;
	}

	state->pos_x += direction * move_speed * input->delta_time;
}

void Game::render(const Input* input, State* state) {

	const float world_to_clip[9] = { 
		2.0f / world_width, 0.0f, 0.0f,
		0.0f, 2.0f / world_height, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	glViewport(0, 0, input->frame_buffer_width, input->frame_buffer_height);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render the quad
	glUseProgram(state->quad_program);
	glBindVertexArray(state->quad_vao);

	int center_pos_location    = glGetUniformLocation(state->quad_program, "center_pos");
	int scale_location         = glGetUniformLocation(state->quad_program, "scale");
	int world_to_clip_location = glGetUniformLocation(state->quad_program, "world_to_clip");

	glUniform2f(center_pos_location, state->pos_x, state->pos_y);
	glUniform2f(scale_location, state->scale_x, state->scale_y);
	glUniformMatrix3fv(world_to_clip_location, 1, GL_FALSE, world_to_clip);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	// @cleanup Find a better way to report errors instead of just at the end of rendering
	while (GLenum error = glGetError()) {
		std::cout << "OpenGL Error: " << error << std::endl;
	}
}