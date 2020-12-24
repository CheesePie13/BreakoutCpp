#version 330 core

uniform float alpha;

out vec4 out_color;

void main() {
	out_color = vec4(0.5f, 0.0f, 0.0f, alpha);
}