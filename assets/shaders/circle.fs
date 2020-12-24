#version 330 core

out vec4 out_color;

in VSTOFS {
	vec2 local_pos;
} in_data;

void main() {
	if (length(in_data.local_pos) > 0.5f) {
		out_color = vec4(0.0f);
	} else {
		out_color = vec4(0.5f, 0.0f, 0.0f, 1.0f);
	}
}