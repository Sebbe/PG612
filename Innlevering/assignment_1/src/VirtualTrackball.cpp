#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>

glm::mat4 quatToMat4(glm::quat q) {
	float length = sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
	float s = 2 / length;

	glm::mat4 mat4(
		1 - (s * ((q.y * q.y) + (q.z * q.z))), s * (q.x * q.y - q.w * q.z),           s * (q.x * q.z + q.w * q.y),           0,
		s * (q.x * q.y + q.w * q.z),           1 - (s * ((q.x * q.x) + (q.z * q.z))), s * (q.y * q.z - q.w * q.x),           0,
		s * (q.x * q.z - q.w * q.y),           s * (q.y * q.z + q.w * q.x),           1 - (s * ((q.x * q.x) + (q.y * q.y))), 0,
		0,                                     0,                                     0,                                     1
	);
	return mat4;
}

VirtualTrackball::VirtualTrackball() {
	quat_old.w = 1.0;
	quat_old.x = 0.0;
	quat_old.y = 0.0;
	quat_old.z = 0.0;
	rotating = false;
}

VirtualTrackball::~VirtualTrackball() {}

void VirtualTrackball::rotateBegin(int x, int y) {
	rotating = true;
	point_on_sphere_begin = getClosestPointOnUnitSphere(x, y);
}

void VirtualTrackball::rotateEnd(int x, int y) {
	rotating = false;
	quat_old = quat_new;
}

glm::mat4 VirtualTrackball::rotate(int x, int y) {
	//If not rotating, simply return the old rotation matrix
	if (!rotating) return quatToMat4(quat_old);

	glm::vec3 point_on_sphere_end; //Current point on unit sphere
	glm::vec3 axis_of_rotation; //axis of rotation
	float theta = 0.0f; //angle of rotation
	
	point_on_sphere_end = getClosestPointOnUnitSphere(x, y);

	/**
	  * Find axis of rotation and angle here. Construct the
	  * rotation quaternion using glm helper functions
	  */
	
	//finda theta
	theta = glm::degrees(glm::acos(glm::dot(point_on_sphere_begin, point_on_sphere_end)));
	//find axis of rotation with the cross product
	axis_of_rotation = glm::cross(point_on_sphere_end, point_on_sphere_begin);

	quat_new = glm::rotate(quat_old, theta, axis_of_rotation);

	//std::cout << "Angle: " << theta << std::endl;
	//std::cout << "Axis: " << axis_of_rotation.x << " " << axis_of_rotation.y << " " << axis_of_rotation.z << std::endl;

	return quatToMat4(quat_new);
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}

glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 coord = glm::vec2(0.0f);
	
	/* shouldnt be hardcoded (should be radius = 0.5 (or whatever the zoom is */
	coord.x = static_cast<float>(x) / w - 0.5f;
	coord.y = 0.5f - static_cast<float>(y) / h;

	//std::cout << "Normalized coordinates: " << coord.x << ", " << coord.y << std::endl;

	return coord;
}

glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float k;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	
	// lenght
	k = sqrt(normalized_coords.x * normalized_coords.x + normalized_coords.y * normalized_coords.y);

	//std::cout << "K: " << k << std::endl;
	/**
	  * Find the point on the unit sphere here from the
	  * normalized window coordinates
	  */

	if(k >= 0.5) {
		point_on_sphere.x = normalized_coords.x / k;
		point_on_sphere.y = normalized_coords.y / k;
		point_on_sphere.z = 0;
	} else {
		point_on_sphere.x = normalized_coords.x * 2;
		point_on_sphere.y = normalized_coords.y * 2;
		point_on_sphere.z = sqrt((1 - 4*k*k));
	}

	//std::cout << "Point on sphere: " << point_on_sphere.x << ", " << point_on_sphere.y << ", " << point_on_sphere.z << std::endl;

	return point_on_sphere;
}