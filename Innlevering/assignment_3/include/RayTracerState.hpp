#ifndef _RAYTRACER_STATE_HPP__
#define _RAYTRACER_STATE_HPP__

#include <memory>

#include <glm/glm.hpp>
#include "SceneObject.hpp"

/**
  * The RayTracerState class keeps track of the state of the ray-tracing:
  * the objects in the scene, camera position, etc, and its main responsibility
  * is to RayTrace the whole scene for each ray.
  */
class RayTracerState {
public:
	RayTracerState(glm::vec3 camera_position) {
		this->camera_position = camera_position;
	}
	
	inline std::vector<std::shared_ptr<SceneObject> >& getScene() { return scene; }
	inline glm::vec3 getCamPos() { return camera_position; }

	/**
	  * Performs raycasting on the scene for the ray ray
	  * @param ray The ray to raycast with
	  * @param t The parameter so that t*ray gives the first intersection point
	  * @return -1 if no intersection found, otherwise the object index in the scene
	  */
	inline glm::vec3 rayTrace(Ray& ray) {
		const float z_offset = 10e-4f;

		float t = -1;
		float t_min = std::numeric_limits<float>::max();
		int k_min=-1;

		if (!ray.isValid()) return glm::vec3(0.0f);

		//Loop through all the objects, to find the closest intersection, if any
		//This is essentially just ray-casting
		for (unsigned int k=0; k<scene.size(); ++k) {
			t = scene.at(k)->intersect(ray);

			if (t > z_offset && t <= t_min) {
				k_min = k;
				t_min = t;
			}
		}

		if (k_min >= 0) {
			return scene.at(k_min)->rayTrace(ray, t_min, *this);
		}
		else {
			return glm::vec3(0.3f);
		}
	}


private:
	std::vector<std::shared_ptr<SceneObject> > scene;
	glm::vec3 camera_position;
};

#endif