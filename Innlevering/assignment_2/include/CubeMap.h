#ifndef CUBEMAP_H__
#define CUBEMAP_H__

#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "GLUtils/GLUtils.hpp"

class CubeMap {
public:
	CubeMap(std::string base_filename, std::string extension);
	~CubeMap();

	void bind(GLenum texture_unit);
	static void unbind();
	void render(const glm::mat4& projection, const glm::mat4& modelview);

private:
	GLuint cubemap;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > vertices;
	std::shared_ptr<GLUtils::BO<GL_ELEMENT_ARRAY_BUFFER> > indices;
	GLuint vao; //< Vertex array object
	std::shared_ptr<GLUtils::Program> cubemap_program; 
	
	static GLubyte quad_indices[];
	static GLfloat quad_vertices[];
};

#endif