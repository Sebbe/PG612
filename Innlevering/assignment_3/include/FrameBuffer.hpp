#ifndef _FRAMEBUFFER_HPP__
#define _FRAMEBUFFER_HPP__

#include <vector>
#include <cassert>

#include <glm/glm.hpp>

/**
  * Our framebuffer class is essentially just a wrapper for a pointer to
  * memory where we store our output pixels
  */
class FrameBuffer {
public:
	FrameBuffer(unsigned int width, unsigned int height) {
		this->width = width;
		this->height = height;
		data.resize(width*height*3);
	}

	inline unsigned int getWidth() { return width; }
	inline unsigned int getHeight() {return height; }
	inline const std::vector<float>& getData() { return data; }

	/**
	  * Sets the pixel at (i, j) to the color (r, g, b).
	  */
	inline void setPixel(unsigned int i, unsigned int j, glm::vec3 color) {
		assert(i >= 0 && i < width);
		assert(j >= 0 && j < height);
		unsigned int index = 3*(i+j*width);
		data.at(index) = color.r;
		data.at(index+1) = color.g;
		data.at(index+2) = color.b;
	}

private:
	std::vector<float> data;
	unsigned int width, height;
};

#endif