#ifndef _VIRTUALTRACKBALL__
#define _VIRTUALTRACKBALL__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

/**
  * Simple class that implements a "virtual trackball"
  * 
  */
class VirtualTrackball {
public:
	VirtualTrackball(bool use_hyperbolic_plane_=false);
	~VirtualTrackball();

	/**
	  * Called when we click the mouse on screen. Finds and
	  * sets rotate_begin_vec to be the vector from the origin
	  * to the closest point on the unit sphere.
	  */
	void rotateBegin(int x, int y);

	/**
	  * Resets the state to move the current camera quaternion
	  */
	void rotateEnd(int x, int y);

	/**
      * Starts zooming state
	  */
	void zoomBegin(int x, int y);

	/**
	  * Ends zooming state
	  */
	void zoomEnd(int x, int y);

	/**
	  * Called when we move the mouse while clicking. Will move
	  * the camera using the "virtual trackball".
	  * Does nothing if we have not called rotateBegin first.
	  */
	void motion(int x, int y, float elapsed);

	/**	
	  * Called every frame. When the mouse is no longer active,
	  * it will give a continuous rotation as defined by the last
	  * mouse movements.
	  */
	void autoRotate(float elapsed);
	
	/**
	  * @return the view matrix representing the current rotation
	  */
	glm::mat4 getMatrix();

	/**
	  * Sets the window size. This is important to be able to 
	  * make sure the virtual trackball fills the whole window
	  */
	void setWindowSize(int w, int h);

private:
	/**
	  * Returns the normalized (x=[-0.5, 0.5], y=[-0.5, 0.5]) window
	  * coordinates from absolute window coordinates (x=[0, w], y=[0, h]).
	  * Note that we flip the y-axis.
	  */
	glm::vec2 getNormalizedWindowCoordinates(int x, int y);

	/**
	  * Function that returns the 3D vector using either a sphere, or
	  * the combination of a sphere and the hyperbolic plane
	  */
	glm::vec3 get3DVector(int x, int y);

	/**
	  * Function that computes the closest 3D point on the unit sphere
	  * from the 2D window position.
	  */
	glm::vec3 getClosestPointOnUnitSphere(int x, int y);

	/**
	  * Alternative to the one above which gives a much better feel
	  */
	glm::vec3 getPointOnSphereOrHyperbolicPlane(int x, int y);

	bool rotating; //Boolean to say if we should rotate or not
	bool zooming;
	unsigned int w; //Window width
	unsigned int h; //Window height

	glm::quat old_camera_quaternion; //Quaternion that represents the old camera position
	glm::quat current_camera_quaternion; //Quaternion that represents the current camera position

	glm::vec3 point_on_sphere_begin; //Vector from origin to first point on the unit sphere
	glm::vec3 point_on_sphere_end; //Last point on unit sphere
	glm::vec3 point_on_sphere_old; //Previous point_on_sphere_end
	float elapsed_old; //Elapsed time when setting point_on_sphere_old. Used to set speed of rotation.

	glm::vec2 zoom_begin;
	float old_zoom;
	float current_zoom;

	bool use_hyperbolic_plane;
};

#endif