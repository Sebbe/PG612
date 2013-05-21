#include "GameManager.h"
#include "GameException.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameException.h"
#include <IL/il.h>
#include <IL/ilu.h>

using std::cerr;
using std::endl;
using GLUtils::BO;
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
	trackball.setWindowSize(window_width, window_height);

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
	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	CHECK_GL_ERRORS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f,
			window_width / (float) window_height, 1.0f, 10.f);
	model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(3));
	view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
}

void GameManager::createSimpleProgram() {
	//Compile shaders, attach to program object, and link
	fresnel_program.reset(new Program("shaders/fresnel.vert", "shaders/fresnel_chromatic.frag"));
	CHECK_GL_ERRORS();

	//Set uniforms for the programs
	fresnel_program->use();
	glUniformMatrix4fv(fresnel_program->getUniform("projection_matrix"), 1, 0, glm::value_ptr(projection_matrix));
	glUniform1i(fresnel_program->getUniform("my_cube"), 0);
	fresnel_program->disuse();
	CHECK_GL_ERRORS();
}

void GameManager::createVAO() {
	glGenVertexArrays(max_vaos, vaos);

	//Load a model into vao 0
	glBindVertexArray(vaos[0]);
	CHECK_GL_ERRORS();
	model.reset(new Model("models/bunny.obj", false));
	model->getVertices()->bind();
	fresnel_program->setAttributePointer("position", 3);
	CHECK_GL_ERRORS();
	model->getNormals()->bind();
	fresnel_program->setAttributePointer("normal", 3);
	CHECK_GL_ERRORS();

	//Unbind and check for errors
	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERRORS();
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
	createSimpleProgram();
	createVAO();
	
	//Initialize IL and ILU
	ilInit();
	iluInit();

	cubemap.reset(new CubeMap("cubemaps/SaintLazarusChurch3/", "jpg"));
}

void GameManager::renderMeshRecursive(MeshPart& mesh, const std::shared_ptr<Program>& program, 
		const glm::mat4& view_matrix, const glm::mat4& model_matrix) {
	//Create modelview matrix
	glm::mat4 meshpart_model_matrix = model_matrix*mesh.transform;
	glm::mat4 modelview_matrix = view_matrix*meshpart_model_matrix;
	glUniformMatrix4fv(program->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(modelview_matrix));
	
	//Create inverse modelview matrix
	glm::mat4 modelview_inverse_matrix = glm::inverse(glm::mat4(modelview_matrix));
	glUniformMatrix4fv(program->getUniform("modelview_inverse_matrix"), 1, 0, glm::value_ptr(modelview_inverse_matrix));
	
	if (mesh.count > 0)
		glDrawArrays(GL_TRIANGLES, mesh.first, mesh.count);
	for (int i=0; i<mesh.children.size(); ++i)
		renderMeshRecursive(mesh.children.at(i), program, view_matrix, meshpart_model_matrix);
}

void GameManager::render() {
	//Clear screen, and set the correct program
	glm::mat4 view_matrix_new = view_matrix*trackball_view_matrix;

	//Render cubemap
	glClear(GL_DEPTH_BUFFER_BIT);
	glDepthMask(GL_FALSE);	
	cubemap->render(projection_matrix, view_matrix_new);
	glDepthMask(GL_TRUE);	
	

	//Render model
	fresnel_program->use();
	cubemap->bind(GL_TEXTURE0);
	glBindVertexArray(vaos[0]);
	renderMeshRecursive(model->getMesh(), fresnel_program, view_matrix_new, model_matrix);
	cubemap->unbind();
	CHECK_GL_ERRORS();

	glBindVertexArray(0);
	CHECK_GL_ERRORS();

	std::cout << 1.0 / my_timer.elapsedAndRestart() << "\r" << std::flush;
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		float elapsed = my_timer.elapsedAndRestart();
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					trackball.rotateBegin(event.motion.x, event.motion.y);
					break;
				case SDL_BUTTON_RIGHT:
					trackball.zoomBegin(event.motion.x, event.motion.y);
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					trackball.rotateEnd(event.motion.x, event.motion.y);
					break;
				case SDL_BUTTON_RIGHT:
					trackball.zoomEnd(event.motion.x, event.motion.y);
					break;
				}
			case SDL_MOUSEMOTION:
				trackball.motion(event.motion.x, event.motion.y, elapsed);
				break;
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
		trackball.autoRotate(elapsed);
		trackball_view_matrix = trackball.getMatrix();
		render();
		SDL_GL_SwapWindow(main_window);
	}
	quit();
}

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}
