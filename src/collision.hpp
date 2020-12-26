#pragma once

#include <math.h>
#include "types.hpp"
#include "vector.hpp"
#include "raycast.hpp"

bool moving_circle_to_vertical_line_collision_check(Vec2 p1, Vec2 p2, float32 radius, float32 line_x_pos, 
		float32* distance, Vec2* point, Vec2* normal) 
{
	if (p1.x < line_x_pos - radius) {
		if (p2.x < line_x_pos - radius) {
			return false;
		}
		return raycast_vertical_line(p1, p2 - p1, line_x_pos - radius, distance, point, normal);
	}

	if (p1.x > line_x_pos + radius) {
		if (p2.x > line_x_pos + radius) {
			return false;
		}
		return raycast_vertical_line(p1, p2 - p1, line_x_pos + radius, distance, point, normal);
	}

	return false;
}

bool moving_circle_to_horizontal_line_collision_check(Vec2 p1, Vec2 p2, float32 radius, float32 line_y_pos, 
		float32* distance, Vec2* point, Vec2* normal) 
{
	if (p1.y < line_y_pos - radius) {
		if (p2.y < line_y_pos - radius) {
			return false;
		}
		return raycast_horizontal_line(p1, p2 - p1, line_y_pos - radius, distance, point, normal);
	}

	if (p1.y > line_y_pos + radius) {
		if (p2.y > line_y_pos + radius) {
			return false;
		}
		return raycast_horizontal_line(p1, p2 - p1, line_y_pos + radius, distance, point, normal);
	}

	return false;
}

// Quick check if there is a possible collision between a circle moving
// from point p1 to point p2 and rectangle.
// Can return false positives.
bool moving_circle_to_retangle_collision_quick_check(Vec2 p1, Vec2 p2, float32 radius, Vec2 center, Vec2 size) {
	Vec2 max = center + (size * 0.5f) + Vec2_ONE * radius;
	if ((p1.x > max.x && p2.x > max.x) ||(p1.y > max.y && p2.y > max.y)) {
		return false;
	}
	
	Vec2 min = center - (size * 0.5f) - Vec2_ONE * radius;
	if ((p1.x < min.x && p2.x < min.x) || (p1.y < min.y && p2.y < min.y)) {
		return false;
	}

	return true;
}

// Check for collision between an axis aligned rectangle and a moving circle
bool moving_circle_to_retangle_collision_check(Vec2 p1, Vec2 p2, float32 radius, Vec2 center, Vec2 size, 
		float32* distance, Vec2* point, Vec2* normal) 
{
	if (!moving_circle_to_retangle_collision_quick_check(p1, p2, radius, center, size)) {
		return false;
	}

	Vec2 delta = p2 - p1;
	Vec2 direction = normalize(delta);
	float32 delta_mag = magnitude(delta);
	
	// Model this as a moving point colliding with a rectangle extruded by the radius size.
	// So the actual collision shape is 4 sides and 4 circles for the corners.
	
	// Left side
	if (direction.x > 0.0f) {
		float32 x = center.x - (size.x * 0.5f) - radius;
		float32 y_min = center.y - (size.y * 0.5f);
		float32 y_max = center.y + (size.y * 0.5f);

		// Because the ray is traveling to the right, if there is a collision
		// with the left side that must be the closest collision point
		if (raycast_vertical_line_segment(p1, direction, x, y_min, y_max, distance, point, normal)) {
			return *distance < delta_mag;
		}
	}

	// Right side
	if (direction.x < 0.0f) {
		float32 x = center.x + (size.x * 0.5f) + radius;
		float32 y_min = center.y - (size.y * 0.5f);
		float32 y_max = center.y + (size.y * 0.5f);

		// Because the ray is traveling to the left, if there is a collision
		// with the right side that must be the closest collision point
		if (raycast_vertical_line_segment(p1, direction, x, y_min, y_max, distance, point, normal)) {
			return *distance < delta_mag;
		}
	}

	// Bottom side
	if (direction.y > 0.0f) {
		float32 y = center.y - (size.y * 0.5f) - radius;
		float32 x_min = center.x - (size.x * 0.5f);
		float32 x_max = center.x + (size.x * 0.5f);

		// Because the ray is traveling upwards, if there is a collision
		// with the bottom side that must be the closest collision point
		if (raycast_horizontal_line_segment(p1, direction, y, x_min, x_max, distance, point, normal)) {
			return *distance < delta_mag;
		}
	}

	// Top side
	if (direction.y < 0.0f) {
		float32 y = center.y + (size.y * 0.5f) + radius;
		float32 x_min = center.x - (size.x * 0.5f);
		float32 x_max = center.x + (size.x * 0.5f);

		// Because the ray is traveling downwards, if there is a collision
		// with the top side that must be the closest collision point
		if (raycast_horizontal_line_segment(p1, direction, y, x_min, x_max, distance, point, normal)) {
			return *distance < delta_mag;
		}
	}

	*distance = INFINITY;
	float32 test_distance;
	Vec2 test_point, test_normal;

	// Bottom-left corner
	if (direction.x > 0.0f || direction.y > 0.0f) {
		Vec2 corner_pos = center - size * 0.5f;
		if (raycast_circle(p1, direction, corner_pos, radius, &test_distance, &test_point, &test_normal) && test_distance < *distance) {
			*distance = test_distance;
			*point = test_point;
			*normal = test_normal;
		}
	}
	
	// Top-left corner
	if (direction.x > 0.0f || direction.y < 0.0f) {
		Vec2 corner_pos = (Vec2){center.x - size.x * 0.5f, center.y + size.y * 0.5f};
		if (raycast_circle(p1, direction, corner_pos, radius, &test_distance, &test_point, &test_normal) && test_distance < *distance) {
			*distance = test_distance;
			*point = test_point;
			*normal = test_normal;
		}
	}
	
	// Bottom-right corner
	if (direction.x < 0.0f || direction.y > 0.0f) {
		Vec2 corner_pos = (Vec2){center.x + size.x * 0.5f, center.y - size.y * 0.5f};
		if (raycast_circle(p1, direction, corner_pos, radius, &test_distance, &test_point, &test_normal) && test_distance < *distance) {
			*distance = test_distance;
			*point = test_point;
			*normal = test_normal;
		}
	}
	
	// Top-right corner
	if (direction.x < 0.0f || direction.y < 0.0f) {
		Vec2 corner_pos = center + size * 0.5f;
		if (raycast_circle(p1, direction, corner_pos, radius, &test_distance, &test_point, &test_normal) && test_distance < *distance) {
			*distance = test_distance;
			*point = test_point;
			*normal = test_normal;
		}
	}

	return *distance < delta_mag;
}
