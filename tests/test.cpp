#include <iostream>
#include <string>
#include "../src/raycast.hpp"

const std::string RED_TEXT = "\033[1;31m";
const std::string GREEN_TEXT = "\033[32m";
const std::string RESET_TEXT = "\033[0m";

const float32 EPSILON = 0.0001f;

std::string to_string(Vec2 v) {
	return "Vec2{" + std::to_string(v.x) + ", " + std::to_string(v.y) + "}";
}

std::string to_string(bool b) {
	return b ? "true" : "false";
}

void verify(std::string* errors, std::string name, float32 expected, float32 actual) {
	if (abs(actual - expected) > EPSILON) {
		*errors += std::string("\t") + name + std::string(" (Expected: ") + std::to_string(expected) 
				+ std::string(", Actual: ") + std::to_string(actual) + ")\n";
	}
}

void verify(std::string* errors, std::string name, Vec2 expected, Vec2 actual) {
	if (abs(actual.x - expected.x) > EPSILON || abs(actual.y - expected.y) > EPSILON) {
		*errors += std::string("\t") + name + std::string(" (Expected: ") + to_string(expected) 
				+ std::string(", Actual: ") + to_string(actual) + ")\n";
	}
}

void verify(std::string* errors, std::string name, bool expected, bool actual) {
	if (abs(actual - expected) > EPSILON) {
		*errors += std::string("\t") + name + std::string(" (Expected: ") + to_string(expected) 
				+ std::string(", Actual: ") + to_string(actual) + ")\n";
	}
}

void test(bool* has_failed, std::string name, std::string errors) {
	if (errors.empty()) {
		std::cout << GREEN_TEXT << "PASSED: " << RESET_TEXT << name << std::endl;
		return;
	}

	*has_failed = true;
	std::cout << RED_TEXT << "FAILED: " << RESET_TEXT << name << std::endl;
	std::cout << errors;
}

std::string test_raycast_circle_hit(Vec2 ray_pos, Vec2 ray_dir, Vec2 circle_pos, float32 circle_radius, 
		Vec2 expected_point, float32 expected_distance, Vec2 expected_normal) 
{
	std::string errors = "";
	float32 distance;
	Vec2 point, normal;
	bool hit = raycast_circle(ray_pos, ray_dir, circle_pos, circle_radius, &distance, &point, &normal);
	verify(&errors, "hit", true, hit);
	verify(&errors, "point", expected_point, point);
	verify(&errors, "distance", expected_distance, distance);
	verify(&errors, "normal", expected_normal, normal);
	return errors;
}

std::string test_raycast_circle_miss(Vec2 ray_pos, Vec2 ray_dir, Vec2 circle_pos, float32 circle_radius) {
	std::string errors = "";
	float32 distance;
	Vec2 point, normal;
	bool hit = raycast_circle(ray_pos, ray_dir, circle_pos, circle_radius, &distance, &point, &normal);
	verify(&errors, "hit", false, hit);
	return errors;
}

int main() {
	bool has_failed = false;
	std::cout << std::endl << "Running Tests..." << std::endl << std::endl; 

	//
	// raycast_circle()
	//
	test(&has_failed, "Raycast Circle Test 1", test_raycast_circle_hit(Vec2(-5.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 0.0f), 1.0f, 
			Vec2(-1.0f, 0.0), 4.0f, Vec2(-1.0f, 0.0f)));

	test(&has_failed, "Raycast Circle Test 2", test_raycast_circle_hit(Vec2(3.0f, 10.0f), Vec2(0.0f, -1.0f), Vec2(0.0f, 0.0f), 5.0f, 
			Vec2(3.0f, 4.0), 6.0f, normalize(Vec2(3.0f, 4.0f))));

	test(&has_failed, "Raycast Circle Test 3", test_raycast_circle_miss(Vec2(0.0f, 0.0f), normalize(Vec2(1.0f, 1.0f)), Vec2(-2.0f, -2.0f), 1.0f));


	if (has_failed) {
		std::cout << std::endl << RED_TEXT << "Tests failed." << RESET_TEXT << std::endl << std::endl;
		return 1;
	} else {
		std::cout << std::endl << GREEN_TEXT << "All tests passed!" << RESET_TEXT << std::endl << std::endl;
		return 0;
	}
}
