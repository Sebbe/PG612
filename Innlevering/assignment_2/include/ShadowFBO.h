#ifndef _SHADOWFBO_HPP__
#define _SHADOWFBO_HPP__

#include "GLUtils/GLUtils.hpp"

class ShadowFBO {
public:
	ShadowFBO(unsigned int width, unsigned int height);
	~ShadowFBO();

	void bind();
	static void unbind();

	unsigned int getWidth() {return width; }
	unsigned int getHeight() {return height; }

	GLuint getTexture() { return texture; }

private:
	GLuint fbo;
	GLuint depth;
	GLuint texture;
	unsigned int width, height;
};

#endif