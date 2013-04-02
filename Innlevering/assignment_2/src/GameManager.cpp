#include "GameManager.h"
#include "GameException.h"
#include "GLUtils/GLUtils.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>
#include <algorithm>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

using std::cerr;
using std::endl;
using GLUtils::BO;
using GLUtils::Program;
using GLUtils::readFile;

const float GameManager::near_plane = 0.5f;
const float GameManager::far_plane = 30.0f;
const float GameManager::fovy = 45.0f;
const float GameManager::cube_scale = GameManager::far_plane*0.75;

const float GameManager::cube_vertices_data[] = {
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,

    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
	
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
	
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,

    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
};

const float GameManager::cube_normals_data[] = {
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,

    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
	
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,

    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};


inline void checkSDLError(int line = -1) {
#ifndef NDEBUG
	const char *error = SDL_GetError();
	if (*error != '\0') {
		std::cout << "SDL Error";
		if (line != -1) {
			std::cout << ", line " << line;
		}
		std::cout << ": " << error << std::endl;
		SDL_ClearError();
	}
#endif
}


GameManager::GameManager() {
	my_timer.restart();
	zoom = 1;
	light.position = glm::vec3(10, 0, 0);
}

GameManager::~GameManager() {
}

void GameManager::createOpenGLContext() {
	//Set OpenGL major an minor versions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // Use double buffering
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16); // Use framebuffer with 16 bit depth buffer
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8); // Use framebuffer with 8 bit for red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8); // Use framebuffer with 8 bit for green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8); // Use framebuffer with 8 bit for blue
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // Use framebuffer with 8 bit for alpha
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	// Initalize video
	main_window = SDL_CreateWindow("NITH - PG612 Assignment 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}
	checkSDLError(__LINE__);

	main_context = SDL_GL_CreateContext(main_window);
	SDL_GL_SetSwapInterval(1);
	checkSDLError(__LINE__);
	
	cam_trackball.setWindowSize(window_width, window_height);

	// Init glew
	// glewExperimental is required in openGL 3.3
	// to create forward compatible contexts 
	glewExperimental = GL_TRUE;
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		std::stringstream err;
		err << "Error initializing GLEW: " << glewGetErrorString(glewErr);
		THROW_EXCEPTION(err.str());
	}

	// Unfortunately glewInit generates an OpenGL error, but does what it's
	// supposed to (setting function pointers for core functionality).
	// Lets do the ugly thing of swallowing the error....
	glGetError();


	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameManager::init() {
	//Create opengl context before we do anything OGL-stuff
	createOpenGLContext();
	
	//Initialize IL and ILU
	ilInit();
	iluInit();

	//Initialize the different stuff we need
	model.reset(new Model("models/bunny.obj", false));
	cube_vertices.reset(new BO<GL_ARRAY_BUFFER>(cube_vertices_data, sizeof(cube_vertices_data)));
	cube_normals.reset(new BO<GL_ARRAY_BUFFER>(cube_normals_data, sizeof(cube_normals_data)));
	
	//Set the matrices we will use
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
	camera.view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
	light.projection = glm::perspective(90.0f, 1.0f, near_plane, far_plane);
	light.view = glm::lookAt(light.position, glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	//Create the random transformations and colors for the bunnys
	srand(static_cast<int>(time(NULL)));
	for (int i=0; i<n_models; ++i) {
		float tx = rand() / (float) RAND_MAX - 0.5f;
		float ty = rand() / (float) RAND_MAX - 0.5f;
		float tz = rand() / (float) RAND_MAX - 0.5f;

		glm::mat4 transformation = model->getTransform();
		transformation = glm::translate(transformation, glm::vec3(tx, ty, tz));

		model_matrices.push_back(transformation);
		model_colors.push_back(glm::vec3(tx+0.5, ty+0.5, tz+0.5));
	}

	//Create the programs we will use
	phong_program.reset(new Program("shaders/phong.vert", "shaders/phong.geom", "shaders/phong.frag"));
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	//Typically diffuse_cubemap and shadowmap
	phong_program->use();
	phong_program->disuse();
	CHECK_GL_ERRORS();
	
	//Set up VAOs and set as input to shaders
	glGenVertexArrays(2, &vao[0]);
	glBindVertexArray(vao[0]);
	model->getVertices()->bind();
	phong_program->setAttributePointer("position", 3);
	model->getNormals()->bind();
	phong_program->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	
	glBindVertexArray(vao[1]);
	cube_vertices->bind();
	phong_program->setAttributePointer("position", 3);
	cube_normals->bind();
	phong_program->setAttributePointer("normal", 3);
	model->getVertices()->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
}

void GameManager::renderColorPass() {
	glViewport(0, 0, window_width, window_height);
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	//Create the new view matrix that takes the trackball view into account
	glm::mat4 view_matrix_new = camera.view*cam_trackball.getTransform();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	phong_program->use();
	
	//Bind shadow map and diffuse cube map

	/**
	  * Render cube
	  */ 
	{
		glBindVertexArray(vao[1]);

		glm::mat4 model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(cube_scale));
		glm::mat4 model_matrix_inverse = glm::inverse(model_matrix);
		glm::mat4 modelview_matrix = view_matrix_new*model_matrix;
		glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
		glm::mat4 modelviewprojection_matrix = camera.projection*modelview_matrix;
		glm::vec3 light_pos = glm::mat3(model_matrix_inverse)*light.position/model_matrix_inverse[3].w;
	
		glUniform3fv(phong_program->getUniform("light_pos"), 1, glm::value_ptr(light_pos));
		glUniform3fv(phong_program->getUniform("color"), 1, glm::value_ptr(glm::vec3(1.0f, 0.8f, 0.8f)));
		glUniformMatrix4fv(phong_program->getUniform("modelviewprojection_matrix"), 1, 0, glm::value_ptr(modelviewprojection_matrix));
		glUniformMatrix4fv(phong_program->getUniform("modelview_matrix_inverse"), 1, 0, glm::value_ptr(modelview_matrix_inverse));

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	/**
	  * Render model
	  * Create modelview matrix and normal matrix and set as input
	  */
	glBindVertexArray(vao[0]);
	for (int i=0; i<n_models; ++i) {
		glm::mat4 model_matrix = model_matrices.at(i);
		glm::mat4 model_matrix_inverse = glm::inverse(model_matrix);
		glm::mat4 modelview_matrix = view_matrix_new*model_matrix;
		glm::mat4 modelview_matrix_inverse = glm::inverse(modelview_matrix);
		glm::mat4 modelviewprojection_matrix = camera.projection*modelview_matrix;
		glm::vec3 light_pos = glm::mat3(model_matrix_inverse)*light.position/model_matrix_inverse[3].w;
	
		glUniform3fv(phong_program->getUniform("light_pos"), 1, glm::value_ptr(light_pos));
		glUniform3fv(phong_program->getUniform("color"), 1, glm::value_ptr(model_colors.at(i)));
		glUniformMatrix4fv(phong_program->getUniform("modelviewprojection_matrix"), 1, 0, glm::value_ptr(modelviewprojection_matrix));
		glUniformMatrix4fv(phong_program->getUniform("modelview_matrix_inverse"), 1, 0, glm::value_ptr(modelview_matrix_inverse));

		glDrawArrays(GL_TRIANGLES, 0, model->getNVertices());
	}
	glBindVertexArray(0);
}

void GameManager::renderShadowPass() {
	//Render the scene from the light, with the lights projection, etc. into the shadow_fbo. Store only the depth values
	//Remember to set the viewport, clearing the depth buffer, etc.
}

void GameManager::render() {
	//Rotate the light a bit
	float elapsed = static_cast<float>(my_timer.elapsedAndRestart());
	glm::mat4 rotation = glm::rotate(elapsed*10.f, 0.0f, 1.0f, 0.0f);
	light.position = glm::mat3(rotation)*light.position;
	light.view = glm::lookAt(light.position,  glm::vec3(0), glm::vec3(0.0, 1.0, 0.0));

	renderShadowPass();
	renderColorPass();
	
	CHECK_GL_ERRORS();
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				cam_trackball.rotateBegin(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEBUTTONUP:
				cam_trackball.rotateEnd(event.motion.x, event.motion.y);
				break;
			case SDL_MOUSEMOTION:
				cam_trackball.rotate(event.motion.x, event.motion.y, zoom);
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_ESCAPE: //Esc
					doExit = true;
					break;
				case SDLK_q: //Ctrl+q
					if (event.key.keysym.mod & KMOD_CTRL) doExit = true;
					break;
				case SDLK_PLUS:
					zoomIn();
					break;
				case SDLK_MINUS:
					zoomOut();
					break;
				}
				break;
			case SDL_QUIT: //e.g., user clicks the upper right x
				doExit = true;
				break;
			}
		}

		//Render, and swap front and back buffers
		render();
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::zoomIn() {
	zoom *= 1.1f;
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
}

void GameManager::zoomOut() {
	zoom = std::max(zoom*0.9f, 0.5f);
	camera.projection = glm::perspective(fovy/zoom,
			window_width / (float) window_height, near_plane, far_plane);
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
