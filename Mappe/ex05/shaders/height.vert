#version 130

uniform sampler2D height_texture;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat3 normal;

in  vec2 in_Position;
out vec2 ex_texcoord;

void main() {
   float height = texture2D(height_texture, in_Position.xy).r;
   vec4 pos = vec4(in_Position.x, 0.2*height, in_Position.y, 1.0);
	gl_Position = projection * view * model * pos;
	ex_texcoord = in_Position;
}