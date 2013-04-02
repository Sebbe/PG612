#ifndef _GLUTILS_HPP__
#define _GLUTILS_HPP__

#include <cstdlib>
#include <sstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define CHECK_GL_ERRORS() GLUtils::checkGLErrors(__FILE__, __LINE__)
#define CHECK_GL_FBO_COMPLETENESS() GLUtils::checkGLFBOCompleteness(__FILE__, __LINE__)

namespace GLUtils {

inline void checkGLErrors(const char* file, unsigned int line) {
	GLenum err = glGetError(); 
    if( err != GL_NO_ERROR ) { 
		std::stringstream log; 
		log << file << '@' << line << ": OpenGL error:" 
             << std::hex << err << " " << gluErrorString(err); 
			 throw std::runtime_error(log.str()); 
    } 
}

inline void checkGLFBOCompleteness(const char* file, unsigned int line) {
	GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE) {
		std::stringstream log; 
		log << file << '@' << line << ": FBO incomplete error:" 
             << std::hex << err << " " << gluErrorString(err); 
			 throw std::runtime_error(log.str()); 
	}
}


}; //Namespace GLUtils

#include "GLUtils/Program.hpp"
#include "GLUtils/BO.hpp"
#include "GLUtils/CubeMap.hpp"

#endif