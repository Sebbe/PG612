#version 150

uniform sampler2D depthTexture;
uniform vec3 color;
smooth in vec3 f_n;
smooth in vec3 f_v;
smooth in vec3 f_l;
smooth in vec4 crd;
smooth in vec3 bary;

out vec4 glFragColor;

float amplify(float d, float scale, float offset) {
	d = scale * d + offset;
	d = clamp(d, 0, 1);
	d = 1 - exp2(-2*d*d);
	return d;
}

void main() {
	vec3 l = normalize(f_l);
    vec3 h = normalize(normalize(f_v)+l);
    vec3 n = normalize(f_n);
	
    float diff = max(0.0f, dot(n, l));
    float spec = pow(max(0.0f, dot(n, h)), 128.0f);

	vec3 colorUse;

	float shadow = textureProj(depthTexture, crd).p;

	shadow = shadow * 0.75 + 0.75;
	colorUse = shadow * color.rgb * diff + spec;

	 //E.g., phong shading;
	float k = min(bary[0],min(bary[1],bary[2])); //minimum of barycentric coordinate
	colorUse = amplify(k, 40, -0.5)*colorUse;

	
	glFragColor = vec4(colorUse, 1.0);
}