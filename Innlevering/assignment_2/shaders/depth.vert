#version 150
uniform mat4 light_transform;
in vec3 position;

void main() {
	gl_Position = light_transform * vec4(position, 1.0);
}