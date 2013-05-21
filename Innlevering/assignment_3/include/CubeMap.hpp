#ifndef _CUBEMAP_HPP__
#define _CUBEMAP_HPP__

#include <string>
#include <limits>
#include <sstream>

#include <glm/glm.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

class CubeMap : public SceneObject {
public:
	CubeMap(std::string posx, std::string negx, 
			std::string posy, std::string negy,
			std::string posz, std::string negz) {
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
		loadImage(posx, this->posx);
		loadImage(negx, this->negx);
		loadImage(posy, this->posy);
		loadImage(negy, this->negy);
		loadImage(posz, this->posz);
		loadImage(negz, this->negz);
	}
	
	/**
	  * Ray-trace function that returns what texel you hit in the
	  * cube map, since any ray will hit some point in the cube map
	  */
	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 out_color(0.1f);
		glm::vec3 dir =  ray.getDirection();

		float sx = 0;
		float tx = 0;

		float ss = 0;
		float ts = 0;
		texture *tex;
		if (glm::abs(dir.x) >= glm::abs(dir.y) && glm::abs(dir.x) >= glm::abs(dir.z)) {

			if (dir.x >= 0) {
				float s = 0.5f - 0.5f * dir.z / dir.x;
				float t = 0.5f - 0.5f * dir.y / dir.x;
				ss = 1.0f - (dir.z / dir.x+ 1.0f) * 0.5f;
				ts = 1.0f - (dir.y / dir.x + 1.0f) * 0.5f;
				

				//out_color = readTexture(posx, ss, ts);
				tex = &posx;
			}
			else {

				ss = 1.0f - (dir.z / dir.x+ 1.0f) * 0.5f;
				ts = (dir.y / dir.x+ 1.0f) * 0.5f;

				//out_color = readTexture(negx, ss, ts);
				tex = &negx;
			}
		}
		else if (glm::abs(dir.y) >= glm::abs(dir.x) && glm::abs(dir.y) >= glm::abs(dir.z)) {

			if (dir.y >= 0) {

				ss = (dir.x / dir.y + 1.0f) * 0.5f;
				ts = (dir.z / dir.y + 1.0f) * 0.5f;
				//out_color = readTexture(posy, ss, ts);
				tex = &posy;
			}
			else {
				ss = 1.0f - (dir.x / dir.y + 1.0f) * 0.5f;
				ts = (dir.z/ dir.y + 1.0f) * 0.5f;

				//out_color = readTexture(negy, ss, ts);
				tex = &negy;
			}
		}
		else {
			if (dir.z >= 0) {

				
				ss = (dir.x / dir.z + 1.0f) * 0.5f;
				ts = 1.0f - (dir.y / dir.z + 1.0f) * 0.5f;

				//out_color = readTexture(posz, ss, ts);
				tex = &posz;
			} else {

				ss = (dir.x / dir.z + 1.0f) * 0.5f;
				ts = (dir.y / dir.z + 1.0f) * 0.5f;

				//out_color = readTexture(negz, ss, ts);
				tex = &negz;
			}
		}
		out_color = readTexture(*tex, ss, ts);
		return out_color;
	}
	
	/**
	  * A ray will always hit the cube map by definition, but the point of intersection
	  * is as far away as possible
	  */
	float intersect(const Ray& r) {
		return std::numeric_limits<float>::max();
	}

private:
	struct texture {
		std::vector<float> data;
		unsigned int width;
		unsigned int height;
	};

	/**
	  * Returns the texel at texture coordinate [s, t] in texture tex
	  */
	static glm::vec3 readTexture(texture& tex, float s, float t) {
		glm::vec3 out_color;
		
		unsigned int xfc = static_cast<unsigned int>(glm::ceil(std::min(s*tex.width, tex.width-1.0f)));
		unsigned int xff = static_cast<unsigned int>(glm::floor(std::min(s*tex.width, tex.width-1.0f)));

		unsigned int yfc = static_cast<unsigned int>(glm::ceil(std::min(t*tex.height, tex.height-1.0f)));
		unsigned int yff = static_cast<unsigned int>(glm::floor(std::min(t*tex.height, tex.height-1.0f)));

		float xf = std::min(s*tex.width, tex.width-1.0f);
		float yf = std::min(t*tex.height, tex.height-1.0f);

		unsigned int xm = static_cast<unsigned int>(xf);
		unsigned int ym = static_cast<unsigned int>(yf);

		unsigned int i0 = (ym*tex.width + xm)*3;
		//unsigned int i0 = (yff * tex.width + xff)*3;
		unsigned int i1 = (yfc * tex.width + xff)*3;
		unsigned int i2 = (yfc * tex.width + xfc)*3;
		unsigned int i3 = (yff * tex.width + xfc)*3;

		for (int k=0; k<3; ++k) {
			float c0 = tex.data.at(i0+k);
			float c1 = tex.data.at(i1+k);
			float c2 = tex.data.at(i2+k);
			float c3 = tex.data.at(i3+k);
			//out_color[k] = ((c0 + c1 + c2 + c3) * 0.25f);
			out_color[k] = c0;
		}

		return out_color;
	}

	/**
	  * Loads an image into memory from file
	  */
	static void loadImage(std::string filename, texture& tex) {
		ILuint ImageName;

		ilGenImages(1, &ImageName); // Grab a new image name.
		ilBindImage(ImageName); 
		
		if (!ilLoadImage(filename.c_str())) {
			ILenum e;
			std::stringstream error;
			while ((e = ilGetError()) != IL_NO_ERROR) {
				error << e << ": " << iluErrorString(e) << std::endl;
			}
			ilDeleteImages(1, &ImageName); // Delete the image name. 
			throw std::runtime_error(error.str());
		}

		tex.width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
		tex.height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
		tex.data.resize(tex.width*tex.height*3);
		
		ilCopyPixels(0, 0, 0, tex.width, tex.height, 1, IL_RGB, IL_FLOAT, tex.data.data());
		ilDeleteImages(1, &ImageName); // Delete the image name. 
	}

	texture posx, negx, posy, negy, posz, negz;
};

#endif
