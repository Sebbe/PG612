#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

inline glm::mat4 quatToMat4(glm::quat m_q) {
	glm::mat4 m;
	float xx = m_q.x*m_q.x;
	float xy = m_q.x*m_q.y;
	float xz = m_q.x*m_q.z;
	float xw = m_q.x*m_q.w;
	float yy = m_q.y*m_q.y;
	float yz = m_q.y*m_q.z;
	float yw = m_q.y*m_q.w;
	float zz = m_q.z*m_q.z;
	float zw = m_q.z*m_q.w;

	m[0][0] = 1.0f-2.0f*(yy+zz);
	m[1][0] =      2.0f*(xy+zw);
	m[2][0] =      2.0f*(xz-yw);

	m[0][1] =      2.0f*(xy-zw);
	m[1][1] = 1.0f-2.0f*(xx+zz);
	m[2][1] =      2.0f*(yz+xw);

	m[0][2] =      2.0f*(xz+yw);
	m[1][2] =      2.0f*(yz-xw);
	m[2][2] = 1.0f-2.0f*(xx+yy);

	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;

	return m;
}

VirtualTrackball::VirtualTrackball(bool use_hyperbolic_plane_) : use_hyperbolic_plane(use_hyperbolic_plane_),
	rotating(false), zooming(false), point_on_sphere_begin(0.0f), point_on_sphere_end(0.0f),
	point_on_sphere_old(0.0f), elapsed_old(1.0f), current_zoom(0.0f) {
	current_camera_quaternion.w = 1.0f;
	current_camera_quaternion.x = 0.0f;
	current_camera_quaternion.y = 0.0f;
	current_camera_quaternion.z = 0.0f;
}

VirtualTrackball::~VirtualTrackball() {}

void VirtualTrackball::rotateBegin(int x, int y) {
	rotating = true;
	old_camera_quaternion = glm::normalize(current_camera_quaternion);
	point_on_sphere_begin = get3DVector(x, y);
	point_on_sphere_old = point_on_sphere_begin;
}

void VirtualTrackball::rotateEnd(int x, int y) {
	rotating = false;
	point_on_sphere_end = get3DVector(x, y);
}

void VirtualTrackball::zoomBegin(int x, int y) {
	zooming = true;
	old_zoom = current_zoom;
	zoom_begin = getNormalizedWindowCoordinates(x, y);
}

void VirtualTrackball::zoomEnd(int x, int y) {
	zooming = false;
}

glm::mat4 VirtualTrackball::getMatrix() {
	return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, current_zoom))*quatToMat4(current_camera_quaternion);
}

void VirtualTrackball::autoRotate(float elapsed) {
	//If not rotating using mouse, simply continue rotating using the last coordinates...
	if (!rotating && !zooming) {
		if (glm::length(point_on_sphere_old-point_on_sphere_end) > 0.01f) {
			float theta = acos(glm::dot(point_on_sphere_old, point_on_sphere_end)) * 180.0f / 3.141592653f;
			glm::vec3 axis_of_rotation = glm::normalize(glm::cross(point_on_sphere_end, point_on_sphere_old));
			current_camera_quaternion = glm::rotate(current_camera_quaternion, theta*elapsed/(10*elapsed_old+1.0e-7f), axis_of_rotation);
		}
	}
}

void VirtualTrackball::motion(int x, int y, float elapsed) {
	//Create rotation defined by mouse movements
	if (rotating) {
		point_on_sphere_old = point_on_sphere_end;
		elapsed_old = elapsed;
		point_on_sphere_end = get3DVector(x, y);
		float theta = acos(glm::dot(point_on_sphere_begin, point_on_sphere_end)) * 180.0f / 3.141592653f;
		glm::vec3 axis_of_rotation = glm::normalize(glm::cross(point_on_sphere_end, point_on_sphere_begin));
		current_camera_quaternion = glm::rotate(old_camera_quaternion, theta, axis_of_rotation);
	}
	if (zooming) {
		glm::vec2 zoom_end = getNormalizedWindowCoordinates(x, y);
		float factor = (zoom_begin.y - zoom_end.y);
		current_zoom = old_zoom + (factor*factor+2.0)*factor;
	}
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}

glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 p;
	p[0] = 2.0f*x/static_cast<float>(w) - 1.0f;
	p[1] = 1.0f - 2.0f*y/static_cast<float>(h);
	return p;
}

glm::vec3 VirtualTrackball::get3DVector(int x, int y) {
	if (use_hyperbolic_plane) {
		return getPointOnSphereOrHyperbolicPlane(x, y);
	}
	else {
		return getClosestPointOnUnitSphere(x, y);
	}
}

glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float r;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	r = glm::length(normalized_coords);

	if (r < 1.0f) { //Ray hits unit sphere
		point_on_sphere[0] = normalized_coords[0];
		point_on_sphere[1] = normalized_coords[1];
		point_on_sphere[2] = sqrt(1 - r*r);

		point_on_sphere = glm::normalize(point_on_sphere);
	}
	else { //Ray falls outside unit sphere
		point_on_sphere[0] = normalized_coords[0]/r;
		point_on_sphere[1] = normalized_coords[1]/r;
		point_on_sphere[2] = 0;            
	}

	return point_on_sphere;
}

glm::vec3 VirtualTrackball::getPointOnSphereOrHyperbolicPlane(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float r;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	r = glm::length(normalized_coords);

	if (r < 0.5f) { //Ray hits unit sphere
		point_on_sphere[0] = normalized_coords[0];
		point_on_sphere[1] = normalized_coords[1];
		point_on_sphere[2] = sqrt(1 - r*r);
	}
	else { //Ray hits hyperbolic plane
		point_on_sphere[0] = normalized_coords[0];
		point_on_sphere[1] = normalized_coords[1];
		point_on_sphere[2] = 0.5f/r;
	}

	point_on_sphere = glm::normalize(point_on_sphere);
	return point_on_sphere;
}