#version 150

uniform sampler2D depthTexture;
uniform samplerCube my_cube;

uniform vec3 color;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
smooth in vec4 crd;

out vec4 out_color;

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	vec3 tex_coord = reflect(-f_v, f_n);
	
    vec3 diff = vec3(texture(my_cube, tex_coord));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	float shadow = textureProj(depthTexture, crd).p;

	shadow = shadow * 0.75 + 0.75;
	out_color = vec4(shadow * color.rgb * diff + spec, 1);
}