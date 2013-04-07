#include "GameManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <IL/il.h>
#include <IL/ilu.h>

using std::cerr;
using std::endl;
using GLUtils::VBO;
using GLUtils::Program;
using GLUtils::readFile;


GameManager::GameManager() {
	my_timer.restart();
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
	main_window = SDL_CreateWindow("NITH - PG612 Example OpenGL Program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!main_window) {
		THROW_EXCEPTION("SDL_CreateWindow failed");
	}

	//Create OpenGL context
	main_context = SDL_GL_CreateContext(main_window);

	// Init glew
	// glewExperimental is required in openGL 3.1 
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
}

void GameManager::setOpenGLStates() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_PRIMITIVE_RESTART);
	glClearColor(0.0, 0.0, 0.5, 1.0);
}

void GameManager::createFBO() {
	//Create a regular texture
	glGenTextures(1, &fbo_texture);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fbo_width, fbo_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	CHECK_GL_ERROR();

	//Create depth renderbuffer
	glGenRenderbuffers(1, &fbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, fbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fbo_width, fbo_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECK_GL_ERROR();

	//Generate an FBO, and attach the texture and render buffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);
	CHECK_GL_ERROR();
	CHECK_GL_FBO_COMPLETENESS();
}

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f, window_width / (float) window_height, 0.2f, 3.f);

	model_matrix = glm::mat4(1.0);
	model_matrix = glm::rotate(model_matrix, 25.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	model_matrix = glm::translate(model_matrix, glm::vec3(-0.5f, -0.5f, -0.5f));

	view_matrix = glm::mat4(1.0f);
	view_matrix = glm::rotate(view_matrix, 25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
}

void GameManager::createHeightProgram() {
	std::string fs_src = readFile("shaders/height.frag");
	std::string vs_src = readFile("shaders/height.vert");
	
	//Compile shaders, attach to program object, and link
	height_program.reset(new Program(vs_src, fs_src));

	//Set uniforms for the program.
	height_program->use();
	
	glUniformMatrix4fv(height_program->getUniform("projection"), 1, 0, glm::value_ptr(projection_matrix));
	glUniformMatrix4fv(height_program->getUniform("view"), 1, 0, glm::value_ptr(view_matrix));
	glUniformMatrix4fv(height_program->getUniform("model"), 1, 0, glm::value_ptr(model_matrix));
	glUniform1i(height_program->getUniform("height_texture"), 0); //< 0 means GL_TEXTURE0
	glUniform1i(height_program->getUniform("color_texture"), 1); //< 1 means GL_TEXTURE1
	
	glUseProgram(0);
	CHECK_GL_ERROR();
}

void GameManager::createFBOProgram() {
	std::string fs_src = readFile("shaders/fbo.frag");
	std::string vs_src = readFile("shaders/fbo.vert");
	
	//Compile shaders, attach to program object, and link
	fbo_program.reset(new Program(vs_src, fs_src));
	fbo_program->use();
	glUniform1i(fbo_program->getUniform("fbo_texture"), 0); //< 0 means GL_TEXTURE0
	glUseProgram(0);
	
	CHECK_GL_ERROR();
}

void GameManager::createHeightVAO() {
	glGenVertexArrays(1, &height_vao);
	glBindVertexArray(height_vao);

	//Generate data
	mesh = createTriangleStripMesh(100, 100);

	//Generate VBO
	glGenBuffers(1, &height_vertex_bo);
	glBindBuffer(GL_ARRAY_BUFFER, height_vertex_bo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size()*sizeof(float), mesh.vertices.data(), GL_STATIC_DRAW);

	//Generate IBO
	glGenBuffers(1, &height_index_bo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, height_index_bo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size()*sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
	
	//Set input to the shader
	height_program->setAttributePointer("in_Position",	2, GL_FLOAT, GL_FALSE, 0, 0);
	CHECK_GL_ERROR();
	
	//Unbind VBOs and VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL_ERROR();
}

void GameManager::createFBOVAO() {
	glGenVertexArrays(1, &fbo_vao);
	glBindVertexArray(fbo_vao);

	//Generate data
	static float positions[8] = {
		-1.0, 1.0,
		-1.0, -1.0,
		1.0, 1.0,
		1.0, -1.0,
	};

	//Generate VBO
	glGenBuffers(1, &fbo_vertex_bo);
	glBindBuffer(GL_ARRAY_BUFFER, fbo_vertex_bo);
	glBufferData(GL_ARRAY_BUFFER, 4*2*sizeof(float), &positions[0], GL_STATIC_DRAW);
	
	//Set input to the shader
	fbo_program->setAttributePointer("in_Position", 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Unbind VBOs and VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void GameManager::init() {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::stringstream err;
		err << "Could not initialize SDL: " << SDL_GetError();
		THROW_EXCEPTION(err.str());
	}
	atexit(SDL_Quit);

	createOpenGLContext();
	setOpenGLStates();
	createMatrices();

	//Initialize geometry and program for heightmap displacement
	createHeightProgram();
	createHeightVAO();

	//Initialize FBO, and program and geometry to render the FBO texture to screen
	createFBO();
	createFBOProgram();
	createFBOVAO();
	
	//Initialize IL and ILU
	ilInit();
	iluInit();

	//Get the textures from file and into an OpenGL texture
	height_texture = loadTexture("ex05_height.bmp");
	color_texture = loadTexture("ex05_tex.bmp");
}

void GameManager::render() {
	//Render to FBO and set the viewport to cover the pixels in the FBO texture
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, fbo_width, fbo_height);

	//Clear screen, and set the correct program
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	height_program->use();

	//Rotate the model	
	//Rotate the model by updating the model matrix
	model_matrix = glm::translate(model_matrix, glm::vec3(0.5f));
	model_matrix = glm::rotate(model_matrix, static_cast<float>(10.0f*my_timer.elapsedAndRestart()), glm::vec3(0.0f, 1.0f, 0.0f));
	model_matrix = glm::translate(model_matrix, glm::vec3(-0.5f));
	glUniformMatrix4fv(height_program->getUniform("model"), 1, 0, glm::value_ptr(model_matrix));
	
	//Bind the textures before rendering
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, height_texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, color_texture);

	//Render geometry
	glPrimitiveRestartIndex(mesh.restart_token);
	glBindVertexArray(height_vao);
	glDrawElements(GL_TRIANGLE_STRIP, mesh.indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	CHECK_GL_ERROR();

	//Unbind the textures
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	




	//Now, render the FBO texture to screen
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	//Set viewport to cover the whole screen
	glViewport(0, 0, window_width, window_height);

	//Clear the depth buffer. Otherwise, our texture might not be rendered due to the depth test
	glClear(GL_DEPTH_BUFFER_BIT);

	//Render the full-screen quad with the FBO texture
	fbo_program->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	glBindVertexArray(fbo_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE) //Esc
					doExit = true;
				if (event.key.keysym.sym == SDLK_q
						&& event.key.keysym.mod & KMOD_CTRL) //Ctrl+q
					doExit = true;
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

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}

GameManager::TerrainMesh GameManager::createTriangleStripMesh(unsigned int nx, unsigned int ny) {
	TerrainMesh mesh;
	
	//Set the restart token: this index will never appear in the mesh.indeces-vector, unless 
	//it means reset, and start a new primitive (triangle strip)
	mesh.restart_token = nx*ny*2+1;

	/**
	  * Fixme: Implement this function properly
	  */
	float dx = 1.0f/static_cast<float>(nx);
	float dy = 1.0f/static_cast<float>(ny);

	//Vertices
	mesh.vertices.reserve((nx+1)*(ny+1));
	for (unsigned int j=0; j<=ny; ++j) {
		for (unsigned int i=0; i<=nx; ++i) {
			mesh.vertices.push_back(i*dx);	//x
			mesh.vertices.push_back(j*dy);	//y
		}
	}

	//Indices
	for (unsigned int j=0; j<ny; ++j) {
		for (unsigned int i=0; i<=nx; ++i) {
			mesh.indices.push_back(    j*(nx+1)+i);
			mesh.indices.push_back((j+1)*(nx+1)+i);
		}
		//Finished with one triangle strip: restart
		mesh.indices.push_back(mesh.restart_token); 
	}

	return mesh;
}

GameManager::TerrainMesh GameManager::createTriangleFanMesh(unsigned int nx, unsigned int ny) {
	TerrainMesh mesh;

	/**
	  * Fixme: Implement this function
	  */
	
	return mesh;
}


GLuint GameManager::loadTexture(std::string filename) {
	std::vector<unsigned char> data;
	ILuint ImageName;
	unsigned int width, height;
	GLuint texture;
		
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
				
	width = ilGetInteger(IL_IMAGE_WIDTH); // getting image width
	height = ilGetInteger(IL_IMAGE_HEIGHT); // and height
	data.resize(width*height*3);
				
	ilCopyPixels(0, 0, 0, width, height, 1, IL_RGB, IL_UNSIGNED_BYTE, data.data());
	ilDeleteImages(1, &ImageName); // Delete the image name. 
		
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERROR();

	return texture;
}