#version 150
uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat4 modelview_inverse_matrix;

in vec3 position;
in vec3 normal;

smooth out vec3 pass_v;
smooth out vec3 pass_n;

void main() {
	vec4 pos = modelview_matrix * vec4(position, 1.0);
	
	float homogeneous_divide = (1.0f/modelview_inverse_matrix[3].w);
	vec3 cam_pos_world = modelview_inverse_matrix[3].xyz*homogeneous_divide;

	pass_v = normalize(cam_pos_world - position.xyz);
	pass_n = normal;

	gl_Position = projection_matrix * pos;
}