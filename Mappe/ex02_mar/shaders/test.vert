#version 130

uniform mat4 projection_matrix;
uniform mat4 modelview_matrix;
uniform mat3 normal_matrix;
uniform vec3 color;
uniform float shininess;

in  vec3 in_Position;
in  vec3 in_Normal;

out vec3 ex_Color;
out vec3 ex_Normal;
out vec3 ex_View;
out vec3 ex_Light;
out float ex_Shininess;

void main() {
	vec4 pos = vec4(in_Position, 1.0);
	gl_Position = projection_matrix * modelview_matrix * pos;
	ex_Color = color;
	ex_Normal = normal_matrix * in_Normal;

	vec3 truePosition = vec3(modelview_matrix * vec4(in_Position, 1.0f));

	ex_View = -truePosition;
	ex_Light = vec3(0.0f, 0.5f, 1.5f) - truePosition;

	ex_Shininess = shininess;
}