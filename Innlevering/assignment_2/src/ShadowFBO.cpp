#include "ShadowFBO.h"
#include "GLUtils/GLUtils.hpp"


ShadowFBO::ShadowFBO(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	
	throw std::runtime_error("ShadowFBO not implemented yet...");

	// Initialize Depth Texture

	// Create FBO and attach buffers

	//Check for completeness
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
}

ShadowFBO::~ShadowFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void ShadowFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void ShadowFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}