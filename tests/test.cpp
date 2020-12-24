#include <iostream>
#include "../src/raycast.hpp"

#define assert(expr) if (!(expr)) {\
	std::cout << "Assert Failed: " << __FILE__ << ":" << __LINE__ << std::endl;\
	abort();\
}

#define assert_msg(expr, msg) if (!(expr)) {\
	std::cout << "Assert Failed: " << __FILE__ << ":" << __LINE__ << " " << (msg) << std::endl;\
	abort();\
}

const float32 epsilon = 0.001f;
bool approx(float32 a, float32 b, float32 delta) {
	return abs(b - a) < delta;
}

void raycast_circle__test_hit(Vec2 ray_pos, Vec2 ray_dir, Vec2 circle_pos, float32 circle_radius, 
		Vec2 expected_point, float32 expected_distance, Vec2 expected_normal, int32 test_id) 
{
	float32 distance;
	Vec2 point, normal;
	bool hit = raycast_circle(ray_pos, ray_dir, circle_pos, circle_radius, &distance, &point, &normal);
	assert_msg(hit, test_id);
	assert_msg(approx(point.x, expected_point.x, epsilon), test_id);
	assert_msg(approx(point.y, expected_point.y, epsilon), test_id);
	assert_msg(approx(distance, expected_distance, epsilon), test_id);
	assert_msg(approx(normal.x, expected_normal.x, epsilon), test_id);
	assert_msg(approx(normal.y, expected_normal.y, epsilon), test_id);
}

void raycast_circle__test_miss(Vec2 ray_pos, Vec2 ray_dir, Vec2 circle_pos, float32 circle_radius, int test_id) {
	float32 distance;
	Vec2 point, normal;
	bool hit = raycast_circle(ray_pos, ray_dir, circle_pos, circle_radius, &distance, &point, &normal);
	assert_msg(!hit, test_id)
}

int main() {

	//
	// raycast_circle()
	//
	// @refactor: Using the test_id to tell which test fails isn't great, it would be better
	// to report a line number and possibly a stack trace.
	raycast_circle__test_hit(Vec2{-5.0f, 0.0f}, Vec2{1.0f, 0.0f}, Vec2{0.0f, 0.0f}, 1.0f, 
			Vec2{-1.0f, 0.0}, 4.0f, Vec2{-1.0f, 0.0f}, 1);

	raycast_circle__test_hit(Vec2{3.0f, 10.0f}, Vec2{0.0f, -1.0f}, Vec2{0.0f, 0.0f}, 5.0f, 
			Vec2{3.0f, 4.0}, 6.0f, normalize(Vec2{3.0f, 4.0f}), 2);

	std::cout << "All tests passed!" << std::endl << std::endl;
}
