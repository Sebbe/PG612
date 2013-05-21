#version 150

uniform samplerCube my_cube;

smooth in vec3 pass_n;
smooth in vec3 pass_v;

out vec4 out_color;

void main() {
	const float eta_air = 1.000293;
	const float eta_carbondioxide = 1.00045;
	const float eta_water = 1.3330;
	const float eta_ethanol = 1.361;
	const float eta_pyrex = 1.470;
	const float eta_diamond = 2.419;

	const float eta0 = eta_air;
	const float eta1 = eta_pyrex;

	const float eta = eta0/eta1;
	const float R0 = pow((eta0-eta1)/(eta0+eta1), 2.0);

	vec3 n = normalize(pass_n);
	vec3 v = normalize(pass_v);

	vec3 texcoord_reflect = reflect(-v, n);
	vec3 texcoord_refract = refract(-v, n, eta);
	
	float fresnel = R0 + (1.0-R0)*pow((1.0-dot(v, n)), 5.0);

	vec4 reflect = texture(my_cube, texcoord_reflect);
	vec4 refract = texture(my_cube, texcoord_refract);

	refract *= vec4(eta, 1.0, eta, 1.0);

	out_color = mix(refract, reflect, fresnel);
}