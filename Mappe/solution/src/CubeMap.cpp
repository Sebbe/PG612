#include "CubeMap.h"

#include "GLUtils/GLUtils.hpp"

#include <glm/gtc/type_ptr.hpp>

using GLUtils::BO;
using GLUtils::Program;

//Vertices to render a quad
GLfloat CubeMap::quad_vertices[] =  {
	-1.f, -1.f,  1.f,
	 1.f, -1.f,  1.f,
	 1.f,  1.f,  1.f,
	-1.f,  1.f,  1.f,

	-1.f, -1.f, -1.f,
	 1.f, -1.f, -1.f,
	 1.f,  1.f, -1.f,
	-1.f,  1.f, -1.f,
};

//Indices to create the quad
//Remember that we see the quad from the inside.... so
//lets draw the triangles counter clockwise as seen from within
GLubyte CubeMap::quad_indices[] = {
	0, 2, 1, 2, 0, 3, //north face
	1, 6, 5, 6, 1, 2, //west face
	5, 7, 4, 7, 5, 6, //south face
	4, 3, 0, 3, 4, 7, //east face
	3, 6, 2, 6, 3, 7, //up face
	0, 5, 4, 5, 0, 1, //down face
};

CubeMap::CubeMap(std::string base_filename, std::string extension) {
	//Load cubemap from file
	cubemap = GLUtils::loadCubeMap(base_filename, extension);

	//Create program
	cubemap_program.reset(new Program("shaders/cubemap.vert", "shaders/cubemap.frag"));
	cubemap_program->use();
	glUniform1i(cubemap_program->getUniform("my_cube"), 0);

	//Create VAO for rendering cubemap
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	CHECK_GL_ERRORS();
	vertices.reset(new BO<GL_ARRAY_BUFFER>(quad_vertices, sizeof(quad_vertices)));
	indices.reset(new BO<GL_ELEMENT_ARRAY_BUFFER>(quad_indices, sizeof(quad_indices)));
	vertices->bind();
	cubemap_program->setAttributePointer("position", 3);
	indices->bind();
	CHECK_GL_ERRORS();

	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

CubeMap::~CubeMap() {

}

void CubeMap::render(const glm::mat4& projection, const glm::mat4& modelview) {
	cubemap_program->use();
	glm::mat4 transform = modelview;
	
	transform[3][0] = 0;
	transform[3][1] = 0;
	transform[3][2] = 0;

	transform = projection*transform;
	glUniformMatrix4fv(cubemap_program->getUniform("transform"), 1, 0, glm::value_ptr(transform));

	bind(GL_TEXTURE0);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	cubemap_program->disuse();
}

void CubeMap::bind(GLenum texture_unit) {
	glActiveTexture(texture_unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
}

void CubeMap::unbind() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
