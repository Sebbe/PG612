#ifndef _TEXTURELOADER_HPP__
#define _TEXTURELOADER_HPP__


#include <sstream>
#include <vector>

#include <IL/il.h>
#include <IL/ilu.h>
#include <GL/glew.h>

namespace GLUtils {

	/**
	  * Loads the six images of a cube map into an OpenGL
	  * cube map texture
	  * @param base_filename the base filename of the files
	  *        to load. The loader then loads the files 
	  *        <base_filename>{posx, negx, posy, negy, posz, negz}.<extension>
	  * @param extension The image filename extension 
	  */
	inline GLuint loadCubeMap(std::string base_filename, std::string extension) {
		const char name_exts[6][5] = {"posx", "negx", "posy", "negy", "posz", "negz"};
		const GLenum faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
		GLuint cube_map_name;
		std::vector<float> data;

		//Allocate texture name and set parameters
		glGenTextures(1, &cube_map_name);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_name);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//Load each face and set texture
		for (int i=0; i<6; ++i) {
			std::stringstream filename;
			ILuint ImageName;
			unsigned int width, height;

			ilGenImages(1, &ImageName); // Grab a new image name.
			ilBindImage(ImageName); 

			filename << base_filename << name_exts[i] << "." << extension;

			if (!ilLoadImage(filename.str().c_str())) {
				ILenum e;
				std::stringstream error;
				while ((e = ilGetError()) != IL_NO_ERROR) {
					error << e << ": " << iluErrorString(e) << std::endl;
				}
				ilDeleteImages(1, &ImageName); // Delete the image name. 
				throw std::runtime_error(error.str());
			}
			
			width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
			height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
			data.resize(width*height*3);
			
			ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data.data());
			ilDeleteImages(1, &ImageName); // Delete the image name. 
			
			glTexImage2D(faces[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
		}

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		return cube_map_name;
	}
}

#endif