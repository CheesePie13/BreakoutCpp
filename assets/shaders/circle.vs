#version 330 core

layout(location = 0) in vec2 local_pos;

uniform mat3 world_to_clip;
uniform vec2 center_pos;
uniform float radius;

out VSTOFS {
	vec2 local_pos;
} out_data;

void main() {
	vec2 world_pos = (local_pos * radius * 2.0f) + center_pos;
	vec3 clip_pos = world_to_clip * vec3(world_pos, 1.0);
	gl_Position = vec4(clip_pos.x, clip_pos.y, 0.0, 1.0);
	out_data.local_pos = local_pos;
}