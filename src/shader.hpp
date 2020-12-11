#pragma once

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "types.h"

void print_shader_logs(uint32 shader) {
	int32 log_size = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
	
	char* log = new char[log_size];
	glGetShaderInfoLog(shader, sizeof(log_size), NULL, log);
	
	std::cout << "Shader Compilation Error: " << log << std::endl;
}

// Need to delete shader after linking with glDeleteShader()
uint32 create_shader(char* shader_code, GLenum shader_type) {
	uint32 shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_code, NULL);
	glCompileShader(shader);

	int32 compile_success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_success);
	if (compile_success) {
		return shader;
	}
	
	print_shader_logs(shader);
	glDeleteShader(shader);
	return 0;
}

uint32 create_shader_program(char* vert_shader_code, char* frag_shader_code) {
	uint32 vert_shader = create_shader(vert_shader_code, GL_VERTEX_SHADER);
	if (vert_shader == 0) {
		return 0;
	}

	uint32 frag_shader = create_shader(frag_shader_code, GL_FRAGMENT_SHADER);
	if (frag_shader == 0) {
		glDeleteShader(vert_shader);
		return 0;
	}

	uint32 program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	int32 link_success;
	glGetProgramiv(program, GL_LINK_STATUS, &link_success);
	if (link_success) {
		return program;
	}

	print_shader_logs(program);
	glDeleteProgram(program);
	return 0;
}