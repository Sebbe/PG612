#version 150

uniform sampler2DShadow depthTexture;
uniform vec3 color;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
//smooth in vec4 f_lightspace;
//smooth in vec4 crd;
//in vec3 crd;
out vec4 out_color;

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	/*
	bool shadow = (textureProj(depthTexture, crd) > crd.z/crd.w);
	
    if(shadow) {
		out_color = vec4((diff*color + spec) * 0.5, 1.0);
	} else {
		out_color = vec4(diff*color + spec, 1.0);
	} */

	out_color = vec4(diff*color + spec, 1.0);
}