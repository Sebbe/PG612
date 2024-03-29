#include "GameManager.h"
#include "GeometryManager.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <assert.h>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLUtils/GlUtils.hpp"
#include "GameException.h"

using std::cerr;
using std::endl;
using GLUtils::VBO;
using GLUtils::Program;
using GLUtils::readFile;

GameManager::GameManager() {
	my_timer.restart();
	render_mode = RENDERMODE_WIREFRAME;
	rotation_quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
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
	glCullFace(GL_BACK);
	glClearColor(backGroundColor.r, backGroundColor.g, backGroundColor.b, 1.0);
}

void GameManager::createMatrices() {
	projection_matrix = glm::perspective(45.0f,
			window_width / (float) window_height, 1.0f, 10.f);
	model_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(3));
	view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.8f));
}

void GameManager::createSimpleProgram() {
	std::string fs_src = readFile("shaders/test.frag");
	std::string vs_src = readFile("shaders/test.vert");

	//Compile shaders, attach to program object, and link
	program.reset(new Program(vs_src, fs_src));

	//Set uniforms for the program.
	program->use();
	glUniformMatrix4fv(program->getUniform("projection_matrix"), 1, 0, glm::value_ptr(projection_matrix));
	program->disuse();
}

void GameManager::createVAO() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	CHECK_GL_ERROR();

	model.reset(new Model("models/bunny.obj", false));
	model->getVertices()->bind();
	program->setAttributePointer("in_Position", 3);
	CHECK_GL_ERROR();
	/**
	  * FIXME 1: Uncomment this part once you have read in the normals properly
	  * using the model loader
	  */
	model->getNormals()->bind();
	program->setAttributePointer("in_Normal", 3);
	CHECK_GL_ERROR();
	
	//Unbind VBOs and VAO
	vertices->unbind(); //Unbinds both vertices and normals
	glBindVertexArray(0);
	CHECK_GL_ERROR();
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

	shininess = 800.0f;
}

void GameManager::renderMeshRecursive(MeshPart& mesh, const std::shared_ptr<Program>& program, 
		const glm::mat4& view_matrix, const glm::mat4& model_matrix, glm::vec3 color) {
	//Create modelview matrix
	glm::mat4 meshpart_model_matrix = model_matrix*mesh.transform;
	glm::mat4 modelview_matrix = view_matrix*meshpart_model_matrix;
	glUniformMatrix4fv(program->getUniform("modelview_matrix"), 1, 0, glm::value_ptr(modelview_matrix));

	//Create normal matrix, the transpose of the inverse
	//3x3 leading submatrix of the modelview matrix
	/**
	  * FIXME 1: Uncomment once you have enabled normal loading etc.
	  * and use of normals in your shader
	  */
	glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(modelview_matrix)));
	glUniformMatrix3fv(program->getUniform("normal_matrix"), 1, 0, glm::value_ptr(normal_matrix));

	glUniform3f(program->getUniform("color"), color.r, color.g, color.b);

	glDrawArrays(GL_TRIANGLES, mesh.first, mesh.count);
	for (unsigned int i=0; i<mesh.children.size(); ++i)
		renderMeshRecursive(mesh.children.at(i), program, view_matrix, meshpart_model_matrix, color);
}

void GameManager::render() {
	//Get elapsed time
	double elapsed = my_timer.elapsedAndRestart();
	float rotate_degrees = static_cast<float>(elapsed) * 45.0f;
	
	//Clear screen, and set the correct program
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program->use();
	
	glUniform1f(program->getUniform("shininess"), shininess);

	//Rotate model.
	/**
	  * FIXME2: Implement this part using a rotation quaternion instead
	  */
	//model_matrix = glm::rotate(model_matrix, rotate_degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	
	rotation_quat = glm::rotate(rotation_quat, rotate_degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	model_matrix = quatToMat4(rotation_quat);

	glm::mat4 modelview_matrix = view_matrix*model_matrix;

	//Render geometry
	/**
	  * FIXME3: Impement different rendering modes here
	  */
	glBindVertexArray(vao);
	switch (render_mode) {
	case RENDERMODE_WIREFRAME:
		renderWireframe(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case RENDERMODE_HIDDEN_LINE:
		renderHiddenline(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case RENDERMODE_FLAT:
		renderFlat(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case RENDERMODE_PHONG:
		renderPhong(glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	default:
		THROW_EXCEPTION("Rendermode not supported");
	}

	glBindVertexArray(0);
	CHECK_GL_ERROR();
}

void GameManager::play() {
	bool doExit = false;

	//SDL main loop
	while (!doExit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {// poll for pending events
			switch (event.type) {
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
				case SDLK_ESCAPE:
					doExit = true;
					break;
				case SDLK_q:
					if (event.key.keysym.mod & KMOD_CTRL) doExit = true; //Ctrl+q
					break;
				case SDLK_RIGHT:
					view_matrix = glm::translate(view_matrix, glm::vec3(-0.1, 0.0, 0.0));
					break;
				case SDLK_LEFT:
					view_matrix = glm::translate(view_matrix, glm::vec3(0.1, 0.0, 0.0));
					break;
				case SDLK_UP:
					view_matrix = glm::translate(view_matrix, glm::vec3(0.0, -0.1, 0.0));
					break;
				case SDLK_DOWN:
					view_matrix = glm::translate(view_matrix, glm::vec3(0.0, 0.1, 0.0));
					break;
				case SDLK_PLUS:
					view_matrix = glm::translate(view_matrix, glm::vec3(0.0, 0.0, 0.1));
					break;
				case SDLK_MINUS:
					view_matrix = glm::translate(view_matrix, glm::vec3(0.0, 0.0, -0.1));
					break;
				case SDLK_1:
					render_mode = RENDERMODE_WIREFRAME;
					break;
				case SDLK_2:
					render_mode = RENDERMODE_HIDDEN_LINE;
					break;
				case SDLK_3:
					render_mode = RENDERMODE_FLAT;
					break;
				case SDLK_4:
					render_mode = RENDERMODE_PHONG;
					break;
				case  SDLK_w:
					if(shininess < 1500)
						shininess += 10;
					std::cout << "Shininess: " << shininess << std::endl;
					break;
				case  SDLK_s:
					if(shininess > 10)
						shininess -= 10;
					std::cout << "Shininess: " << shininess << std::endl;
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

void GameManager::quit() {
	std::cout << "Bye bye..." << std::endl;
}


glm::mat4 GameManager::quatToMat4(glm::quat q) {
	
	float length = sqrt(pow(q.x, 2) + pow(q.y, 2) + pow(q.z, 2) + pow(q.w, 2));
	float s = 2 / length;

	glm::mat4 mat4(
		1 - (s * (pow(q.y, 2) + pow(q.z, 2))), s * (q.x * q.y - q.w * q.z),           s * (q.x * q.z + q.w * q.y),           0,
		s * (q.x * q.y + q.w * q.z),           1 - (s * (pow(q.x, 2) + pow(q.z, 2))), s * (q.y * q.z - q.w * q.x),           0,
		s * (q.x * q.z - q.w * q.y),           s * (q.y * q.z + q.w * q.x),           1 - (s * (pow(q.x, 2) + pow(q.y, 2))), 0,
		0,                                     0,                                     0,                                     1
	);
	return mat4;
}

void GameManager::renderWireframe(glm::vec3 color)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	renderMeshRecursive(model->getMesh(), program, view_matrix, model_matrix, color);
}

void GameManager::renderPhong(glm::vec3 color)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	renderMeshRecursive(model->getMesh(), program, view_matrix, model_matrix, color);
}

void GameManager::renderFlat(glm::vec3 color)
{

}

void GameManager::renderHiddenline(glm::vec3 color)
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	renderPhong(backGroundColor);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-0.0f, -0.0f);
	renderWireframe(color);
	glDisable(GL_POLYGON_OFFSET_LINE);
}

const glm::vec3 GameManager::backGroundColor = glm::vec3(0.0f, 0.0f, 0.0f);