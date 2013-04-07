#version 130

uniform sampler2D fbo_texture;
in vec2 ex_texcoord;
out vec4 res_Color;

void main() {
    const int num_bins = 4; //Four bit colors FTW!
    vec3 color = texture2D(fbo_texture, ex_texcoord.xy).rgb;
    color = ivec3(color * num_bins); //Integer thresholding to get eighties-look
    color = color / float(num_bins);
    res_Color = vec4(color, 1.0); 
}