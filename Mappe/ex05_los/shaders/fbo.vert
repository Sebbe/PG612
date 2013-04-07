#version 130

in  vec2 in_Position;
out vec2 ex_texcoord;

void main() {
	gl_Position = vec4(in_Position.x, in_Position.y, 0.5, 1);
	ex_texcoord = 0.5*in_Position+vec2(0.5);
}