#pragma once

#include <iostream>

/**
 * Load a file (remember to call free_file() after)
 */
char* load_file(const char* path) {
	FILE* file = fopen(path, "r");
	if (file == NULL) {
		std::cout << "Failed to open file: " << path << std::endl;
		return NULL;
	}

	fseek(file, 0L, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	char* file_data = new char[size + 1];
	size_t read_size = fread(file_data, 1, size + 1, file);
	fclose(file);

	file_data[read_size] = '\0';

	return file_data;
}

/**
 * Free a file loaded with load_file()
 */
void free_file(char* file) {
	delete file;
}
