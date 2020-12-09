#include <iostream>

#include "game.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void error_callback(int error, const char* description) {
	std::cerr << "Error: " << description << std::endl;
}

int main() {

	//
	// GLFW Window setup
	//
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		std::cout << "Failed to initialize glfw." << std::endl;
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window) {
		std::cout << "Failed to create window." << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	// Enable V-sync
	glfwSwapInterval(1);

	//
	// Initialize Game
	//
	GameInput gameInput;
	gameInput.deltaTime = 1.0 / 60.0;
	gameInput.leftKeyPressed = false;
	gameInput.rightKeyPressed = true;

	GameState* gameState = game_init(&gameInput);
	
	//
	// Game Loop
	//
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		game_render(&gameInput, gameState);
		game_update(&gameInput, gameState);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}