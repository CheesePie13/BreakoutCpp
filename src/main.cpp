#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.hpp"
#include <unistd.h>

#ifdef MACOS
#include <mach-o/dyld.h>
#endif

Game::Input game_input;

void error_glfw_callback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

void frame_buffer_size_glfw_callback(GLFWwindow* window, int width, int height) {
	game_input.frame_buffer_width  = width;
	game_input.frame_buffer_height = height;
}

int main() {
	std::cout << "Starting..." << std::endl;

	// Set working directory to be the directory the executable is in.
	#ifdef MACOS
	uint32 exec_path_size = 1024;
	char exec_path[exec_path_size];
	
	_NSGetExecutablePath(exec_path, &exec_path_size);
	*strrchr(exec_path, '/') = '\0'; // Remove file name after last '/'
	chdir(exec_path);
	std::cout << "Working Directory: " << exec_path << std::endl;
	#elif WINDOWS
	// @todo: Set working directory on windows
	#endif

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
	
	GLFWwindow* window = glfwCreateWindow(1280, 720, "BreakoutCpp", NULL, NULL);
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
	glfwGetFramebufferSize(window, &game_input.frame_buffer_width, &game_input.frame_buffer_height);

	Game::State* game_state = Game::init(&game_input);
	if (game_state == NULL) {
		std::cout << "Failed to initialize game." << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//
	// Game Loop
	//
	float64 prev_frame_time;
	float64 cur_frame_time;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Time
		prev_frame_time = cur_frame_time;
		cur_frame_time = glfwGetTime();
		game_input.frame_time = cur_frame_time;
		game_input.delta_time = cur_frame_time - prev_frame_time;
		
		// Controls
		game_input.left_key_pressed = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
		game_input.right_key_pressed = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

		Game::update(&game_input, game_state);
		Game::render(&game_input, game_state);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}