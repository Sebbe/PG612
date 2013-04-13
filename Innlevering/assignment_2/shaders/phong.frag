#version 150

uniform sampler2D depthTexture;
uniform samplerCube my_cube;

uniform vec3 color;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
smooth in vec4 crd;
//smooth in vec4 f_lightspace;
//smooth in vec4 crd;
//in vec3 crd;
out vec4 out_color;

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	vec3 tex_coord = reflect(-f_v, f_n);
	
    vec3 diff = texture(my_cube, tex_coord).xyz;
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);
	
	bool shadow = false;
	 if (crd.x >= 0 && crd.x < crd.w && crd.y >= 0 && crd.y < crd.w)
		shadow = (textureProj(depthTexture, crd).r > crd.z/crd.w);
	
	/** Mest sansynlig en fortegns feil et sted **/
    if(!shadow) {
		out_color = vec4((diff*color + spec) * 0.2, 1.0);
	} else {
		out_color = vec4(diff*color + spec, 1.0);
	}
}