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
	const float eta1 = eta_ethanol;
	
	const float D0 = pow(1.01, 1.0+5*(eta1-eta0))-1.0;//log(1+eta1-eta0);
	const vec3 eta = vec3(eta0, eta0+D0, eta0+2*D0)/vec3(eta1);
	const float R0 = pow((eta0-eta1)/(eta0+eta1), 2.0);

	vec3 n = normalize(pass_n);
	vec3 v = normalize(pass_v);
	
	vec3 texcoord_reflect = reflect(-v, n);
	vec3 texcoord_refract_r = refract(-v, n, eta.r);
	vec3 texcoord_refract_g = refract(-v, n, eta.g);
	vec3 texcoord_refract_b = refract(-v, n, eta.b);
	
	float fresnel = R0 + (1.0-R0)*pow((1.0-dot(v, n)), 5.0);

	vec4 reflect = texture(my_cube, texcoord_reflect);
	float refract_r = texture(my_cube, texcoord_refract_r).r;
	float refract_g = texture(my_cube, texcoord_refract_g).g;
	float refract_b = texture(my_cube, texcoord_refract_b).b;
	vec4 refract = vec4(refract_r, refract_g, refract_b, 1.0);

	refract *= vec4(eta.r, 1.0, eta.r, 1.0);

	out_color = mix(refract, reflect, fresnel);
}