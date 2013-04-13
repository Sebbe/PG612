#include "ShadowFBO.h"
#include "GLUtils/GLUtils.hpp"


ShadowFBO::ShadowFBO(unsigned int width, unsigned int height) {
	this->width = width;
	this->height = height;
	
	//throw std::runtime_error("ShadowFBO not implemented yet...");

	//Create a depth component texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
	width, height, 0, GL_DEPTH_COMPONENT,
	GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generate an FBO, and attach the depth texture
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glDrawBuffer(GL_NONE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
	GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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