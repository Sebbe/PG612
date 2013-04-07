#version 130

uniform sampler2D color_texture;
in vec2 ex_texcoord;
out vec4 res_Color;

void main() {
    res_Color = texture2D(color_texture, ex_texcoord.xy);
}