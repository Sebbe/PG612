#version 150
 
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

smooth in vec3 g_n[3];
smooth in vec3 g_v[3];
smooth in vec3 g_l[3];
smooth in vec4 g_crd[3];
//smooth in vec4 g_lightspace[3];

smooth out vec3 f_n;
smooth out vec3 f_v;
smooth out vec3 f_l;
smooth out vec4 crd;
//smooth out vec4 f_lightspace;

smooth out vec3 bary;
 
void main() {
	for(int i = 0; i < gl_in.length(); i++) {
		f_n = g_n[i];
		f_v = g_v[i];
		f_l = g_l[i];
		crd = g_crd[i];
		if(i == 0) bary = vec3(1,0,0);
		if(i == 1) bary = vec3(0,1,0);
		if(i == 2) bary = vec3(0,0,1);

		//f_lightspace = g_lightspace[i];
		gl_Position =  gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}