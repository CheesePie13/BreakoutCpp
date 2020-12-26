#include "types.hpp"
#include "vector.hpp"

// Find the intersection points between a circle and a ray
bool raycast_circle(Vec2 ray_pos, Vec2 ray_dir, Vec2 circle_pos, float32 circle_radius, 
		float32* distance, Vec2* point, Vec2* normal)
{
	// Rename some variables for convenience
	Vec2 a = ray_pos;
	Vec2 b = circle_pos;

	// Project ab onto the ray, this will give the closest point to b on the ray (c).
	// This point c is the midpoint between the 2 possible intersection points.
	Vec2 ab = b - a;
	Vec2 c = a + dot(ab, ray_dir) * ray_dir;

	// If the distance between point c and the circle's center is more than the radius
	// there is no intersection point
	float32 mag_bc = magnitude(c - b);
	if (mag_bc > circle_radius) {
		return false;
	}

	// To find the intersection point (d) that is closer to a, use pythagorean theorem on
	// triangle bcd, bc is known and bd has a length of circle_radius so we can find length cd
	float32 mag_cd = sqrt(circle_radius * circle_radius - mag_bc * mag_bc);
	Vec2 d = c - ray_dir * mag_cd;

	// Make sure the intersection point is not behind the ray
	float32 mag_ad = dot(ray_dir, d - a); // Avoids sqrt in magnitude() and if negative we know it's behind
	if (mag_ad < 0) {
		return false;
	}

	*distance = mag_ad;
	*point = d;
	*normal = normalize(d - b);
	return true;
}

bool raycast_horizontal_line(Vec2 ray_pos, Vec2 ray_dir, float32 y,
		float32* distance, Vec2* point, Vec2* normal)
{
	Vec2 delta; // Vector between ray_pos and intersection point
	delta.y = y - ray_pos.y;

	// Ray y direction needs to be in the same direction as the delta
	if (delta.y * ray_dir.y <= 0.0f) {
		return false;
	}

	delta.x = delta.y * (ray_dir.x / ray_dir.y);

	*distance = magnitude(delta);
	point->x = ray_pos.x + delta.x;
	point->y = y;
	*normal = delta.y > 0 ? (Vec2){0.0f, -1.0f} : (Vec2){0.0f, 1.0f};
	return true;
}

bool raycast_horizontal_line_segment(Vec2 ray_pos, Vec2 ray_dir, float32 y, float32 x_min, float32 x_max,
		float32* distance, Vec2* point, Vec2* normal)
{
	if (!raycast_horizontal_line(ray_pos, ray_dir, y, distance, point, normal)) {
		return false;
	}

	if (point->x < x_min || point->x > x_max) {
		return false;
	}

	return true;
}

bool raycast_vertical_line(Vec2 ray_pos, Vec2 ray_dir, float32 x,
		float32* distance, Vec2* point, Vec2* normal)
{
	Vec2 delta; // Vector between ray_pos and intersection point
	delta.x = x - ray_pos.x;

	// Ray y direction needs to be in the same direction as the delta
	if (delta.x * ray_dir.x <= 0.0f) {
		return false;
	}

	delta.y = delta.x * (ray_dir.y / ray_dir.x);

	point->x = x;
	point->y = ray_pos.y + delta.y;
	*distance = magnitude(delta);
	*normal = delta.x > 0 ? (Vec2){-1.0f, 0.0f} : (Vec2){1.0f, 0.0f};
	return true;
}

bool raycast_vertical_line_segment(Vec2 ray_pos, Vec2 ray_dir, float32 x, float32 y_min, float32 y_max,
		float32* distance, Vec2* point, Vec2* normal)
{
	if (!raycast_vertical_line(ray_pos, ray_dir, x, distance, point, normal)) {
		return false;
	}

	if (point->y < y_min || point->y > y_max) {
		return false;
	}

	return true;
}
