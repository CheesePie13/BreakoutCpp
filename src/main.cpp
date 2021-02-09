#include <iostream>
#include <unistd.h>

// External defines: 
// - VERSION: String with version number (ex. "v1.0")
// - WINDOWS: True if windows build
// - MACOS: True if mac build

#ifndef VERSION
#define VERSION "Unknown Version"
#endif

#ifdef WINDOWS
#include <Windows.h>
#elif MACOS
#include <mach-o/dyld.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.hpp"

GLFWwindow* window;
Game::Input game_input;

void error_glfw_callback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

void frame_buffer_size_glfw_callback(GLFWwindow* window, int width, int height) {
	game_input.frame_buffer_size.x = width;
	game_input.frame_buffer_size.y = height;
}

void update_ui(int32 score, int32 lives, const char* info) {
	char window_title[1024];
	if (info == NULL) {
		snprintf(window_title, sizeof(window_title), "BreakoutCpp (" VERSION ") - Score: %d, Lives: %d ", score, lives);
	} else {
		snprintf(window_title, sizeof(window_title), "BreakoutCpp (" VERSION ") - Score: %d, Lives: %d - %s", score, lives, info);
	}
	glfwSetWindowTitle(window, window_title);
}

/**
 * Program entry point
 */
int main() {

	std::cout << "Starting..." << std::endl;

	// Set working directory to be the directory the executable is in.
	uint32 exec_path_size = 1024;
	char exec_path[exec_path_size];

	#ifdef WINDOWS
	GetModuleFileName(NULL, exec_path, exec_path_size);
	*strrchr(exec_path, '\\') = '\0'; // Remove file name after last '/'
	#elif MACOS
	_NSGetExecutablePath(exec_path, &exec_path_size);
	*strrchr(exec_path, '/') = '\0'; // Remove file name after last '/'
	#endif

	chdir(exec_path);
	std::cout << "Working Directory: " << exec_path << std::endl;

	//
	// GLFW Window setup
	//
	glfwSetErrorCallback(error_glfw_callback);

	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw." << std::endl;
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window = glfwCreateWindow(1280, 720, "BreakoutCpp (" VERSION ")", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// Force Aspect Ratio to 16 by 9
	glfwSetWindowAspectRatio(window, 16, 9);

	// Set up frame size change callback
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_glfw_callback);

	// Enable V-sync
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		glfwTerminate();
		return 1;
	}

	//
	// Initialize Game
	//
	game_input.delta_time        = 1.0 / 60.0;
	game_input.left_key_pressed  = false;
	game_input.right_key_pressed = true;
	glfwGetFramebufferSize(window, &game_input.frame_buffer_size.x, &game_input.frame_buffer_size.y);
	game_input.update_ui = update_ui;

	Game::Data* game_data = Game::init(&game_input);
	if (game_data == NULL) {
		std::cout << "Failed to initialize game." << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//
	// Game Loop
	//
	float64 prev_frame_time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Time
		float64 cur_frame_time = glfwGetTime();
		game_input.frame_time = cur_frame_time;
		game_input.delta_time = cur_frame_time - prev_frame_time;

		// If the frame took a really long time we are probably debugging, 
		// so make the frame delta a reasonable value
		if (game_input.delta_time > 1.0f) {
			game_input.delta_time = 0.166666f;
		}

		prev_frame_time = cur_frame_time;
		
		// Controls
		game_input.left_key_pressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS 
				|| glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;

		game_input.right_key_pressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS
				|| glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

		game_input.start_key_pressed_prev = game_input.start_key_pressed;
		game_input.start_key_pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

		// Update and render the game
		Game::update(&game_input, game_data);
		Game::render(&game_input, game_data);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}