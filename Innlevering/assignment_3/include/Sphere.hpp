#ifndef _SPHERE_HPP__
#define _SPHERE_HPP__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"

#include <glm/glm.hpp>

/**
  * The sphere is a scene object that is easy to work with. We have
  * simple analytical formulations for both the intersection with a ray and
  * for computation of the normal at any point
  */
class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 center, float radius, std::shared_ptr<SceneObjectEffect> effect) {
		this->p = center;
		this->r = radius;
		this->effect = effect;
	}

	/**
	  * Computes the ray-sphere intersection
	  */
	float intersect(const Ray& r) {
		const float z_offset = 10e-4f;
		const glm::vec3 d = r.getDirection();
		const glm::vec3 p0 = r.getOrigin();
		float a = glm::dot(d, d);
		float b = 2.0f*glm::dot(d, (p0-p));
		float c = glm::dot(p0-p, p0-p)-this->r*this->r;
		
		static int initialized=0;
		if (!initialized) {
			std::cerr << "The Sphere::intersect(...) function is not implemented properly!" << std::endl;
			++initialized;
		}

		//Just test whether or not we hit the ray to display something at least
		if ((b*b-4.0f*a*c) >= 0.0f) {
			return 1;
		}
		else {
			return -1;
		}
	}
	
	/**
	  * Computes normal for an intersection point on a sphere
	  */
	const glm::vec3 computeNormal(const Ray& r, const float& t) {
		static int initialized=0;
		if (!initialized) {
			std::cerr << "The Sphere::computeNormal(...) function is not implemented properly!" << std::endl;
			++initialized;
		}
		return glm::vec3(0);
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		glm::vec3 normal = computeNormal(ray, t);
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 p; //< center of sphere
	float r;   //< sphere radius
};

#endif	