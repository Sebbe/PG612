#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include <memory>

#include <GL/glew.h>
#include <SDL.h>
#include <glm/glm.hpp>

#include "Timer.h"
#include "GLUtils/GLUtils.hpp"
#include "Model.h"
#include "VirtualTrackball.h"
#include "ShadowFBO.h"

/**
 * This class handles the game logic and display.
 * Uses SDL as the display manager, and glm for 
 * vector and matrix computations
 */
class GameManager {
public:

	/**
	 * Constructor
	 */
	GameManager();

	/**
	 * Destructor
	 */
	~GameManager();

	/**
	 * Initializes the game, including the OpenGL context
	 * and data required
	 */
	void init();

	/**
	 * The main loop of the game. Runs the SDL main loop
	 */
	void play();

	/**
	 * Quit function
	 */
	void quit();

	/**
	 * Function that handles rendering into the OpenGL context
	 */
	void render();

	/**
	  * Function that renders a shadow pass
	  */
	void renderShadowPass();

	/**
	  * Function that renders to screen
	  */
	void renderColorPass();

protected:
	/**
	 * Creates the OpenGL context using SDL
	 */
	void createOpenGLContext();

	static const unsigned int window_width = 800;
	static const unsigned int window_height = 600;
	
	static const unsigned int shadow_map_width = 1024;
	static const unsigned int shadow_map_height = 1024;

	static const unsigned int n_models = 20;

	static const float near_plane;
	static const float far_plane;
	static const float fovy;
	static const float cube_scale;
	
	static const float cube_vertices_data[];
	static const float cube_normals_data[];

private:
	void zoomIn();
	void zoomOut();
	
	GLuint vao[2]; //< Vertex array objects
	std::shared_ptr<GLUtils::Program> phong_program, wireframe_program, exploded_view_program, shadow_program;
	std::shared_ptr<GLUtils::CubeMap> diffuse_cubemap;
	std::shared_ptr<GLUtils::BO<GL_ARRAY_BUFFER> > cube_vertices, cube_normals;

	std::shared_ptr<Model> model;
	std::shared_ptr<ShadowFBO> shadow_fbo;

	Timer my_timer; //< Timer for machine independent motion
	float zoom; //< Zoom factor

	struct {
		glm::vec3 position; //< Light position for shading etc
		glm::mat4 projection;
		glm::mat4 view;
	} light;

	struct {
		glm::mat4 projection;
		glm::mat4 view;
	} camera;

	std::vector<glm::mat4> model_matrices; //< OpenGL model transformation matrix
	std::vector<glm::vec3> model_colors;

	SDL_Window* main_window; //< Our window handle
	SDL_GLContext main_context; //< Our opengl context handle 
	
	VirtualTrackball cam_trackball;
};

#endif // _GAMEMANAGER_H_
