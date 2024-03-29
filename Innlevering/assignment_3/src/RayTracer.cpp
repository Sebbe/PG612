#include "RayTracer.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <sys/stat.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "CubeMap.hpp"

RayTracer::RayTracer(unsigned int width, unsigned int height) {
	const glm::vec3 camera_position(0.0f, 0.0f, 10.0f);

	//Initialize framebuffer and virtual screen
	fb.reset(new FrameBuffer(width, height));
	float aspect = width/static_cast<float>(height);
	screen.top = 1.0f;
	screen.bottom = -1.0f;
	screen.right = aspect;
	screen.left = -aspect;
	
	//Initialize state
	state.reset(new RayTracerState(camera_position));
	
	//Initialize IL and ILU
	ilInit();
	iluInit();
}

void RayTracer::addSceneObject(std::shared_ptr<SceneObject>& o) {
	state->getScene().push_back(o);
}

void RayTracer::render() {
	//For every pixel, ray-trace using multiple CPUs
#ifdef _OPENMP
#pragma omp parallel for
	for (int j=0; j<static_cast<int>(fb->getHeight()); ++j) {
#else
	for (unsigned int j=0; j<fb->getHeight(); ++j) {
#endif
		for (unsigned int i=0; i<fb->getWidth(); ++i) {
			glm::vec3 out_color(0.0, 0.0, 0.0);
			float t = std::numeric_limits<float>::max();
			float x1, y1, x2, y2, x3, y3, x4, y4, z;

			// Create the ray using the view screen definition 
			x1 = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
			y1 = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;

			x2 = (i-0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
			y2 = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;

			x3 = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
			y3 = (j+0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;

			x4 = (i+0.25f)*(screen.right-screen.left)/static_cast<float>(fb->getWidth()) + screen.left;
			y4 = (j-0.25f)*(screen.top-screen.bottom)/static_cast<float>(fb->getHeight()) + screen.bottom;
			z = -1.0f;
			
			glm::vec3 directionR1 = glm::vec3(x1, y1, z);
			Ray r1 = Ray(state->getCamPos(), directionR1);

			glm::vec3 directionR2 = glm::vec3(x2, y2, z);
			Ray r2 = Ray(state->getCamPos(), directionR2);

			glm::vec3 directionR3 = glm::vec3(x3, y3, z);
			Ray r3 = Ray(state->getCamPos(), directionR3);

			glm::vec3 directionR4 = glm::vec3(x4, y4, z);
			Ray r4 = Ray(state->getCamPos(), directionR4);

			//Now do the ray-tracing to shade the pixel
			out_color = (state->rayTrace(r1) + state->rayTrace(r2) + state->rayTrace(r3) + state->rayTrace(r4)) * 0.25f;

			fb->setPixel(i, j, out_color);
		}
	}
}

void RayTracer::save(std::string basename, std::string extension) {
	ILuint texid;
	struct stat buffer;
	int i;
	std::stringstream filename;

	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	//Create image
	ilGenImages(1, &texid);
	ilBindImage(texid);
	//FIXME: Ugly const cast:( DevILs fault, unfortunately
	ilTexImage(fb->getWidth(), fb->getHeight(), 1, 3, IL_RGB, IL_FLOAT, const_cast<float*>(fb->getData().data()));

	//Find a unique filename...
	for (i=0; i<10000; ++i) {
		filename.str("");
		filename << basename << std::setw(4) << std::setfill('0') << i << "." << extension;
		if (stat(filename.str().c_str(), &buffer) != 0) break;
	}

	if (i == 10000) {
		std::stringstream log;
		log << "Unable to find unique filename for " << basename << "%d." << extension;
		throw std::runtime_error(log.str());
	}

	if (!ilSaveImage(filename.str().c_str())) {
		std::stringstream log;
		log << "Unable to save " << filename.str();
		throw std::runtime_error(log.str());
	}
	else {
		std::cout << "Saved " << filename.str() << std::endl;
	}

	ilDeleteImages(1, &texid);
}