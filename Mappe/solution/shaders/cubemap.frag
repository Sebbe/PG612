#version 150

uniform samplerCube my_cube;
smooth in vec3 texcoord;
out vec4 out_color;

void main() {
	out_color = vec4(texture(my_cube, texcoord).rgb, 1.0);
}