#ifndef _TRIANGLE_H__
#define _TRIANGLE_H__

#include "RayTracerState.hpp"
#include "SceneObject.hpp"
#include "SceneObjectEffect.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <cmath>
/**
  * The sphere is a scene object that is easy to work with. We have
  * simple analytical formulations for both the intersection with a ray and
  * for computation of the normal at any point
  */
class Triangle : public SceneObject {
public:
	Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, std::shared_ptr<SceneObjectEffect> effect) {
		this->a = a;
		this->b = b;
		this->c = c;
		this->effect = effect;
		glm::vec3 ab = this->b - this->a;
		glm::vec3 ac = this->c - this->a;

		normal = glm::normalize(glm::cross(ab, ac));
	}

	/**
	  * Computes the ray-sphere intersection
	  */
	float intersect(const Ray& r) {
		float t = glm::dot((a - r.getOrigin()), normal) / (glm::dot(r.getDirection(), normal));
        glm::vec3 q = r.getOrigin() + t * r.getDirection();
		
		if (glm::dot(glm::cross(q-a, c-a), normal) > 0 &&
                 glm::dot(glm::cross(q-b, a-b), normal) > 0 &&
                 glm::dot(glm::cross(q-c, b-c), normal) > 0) {
				return t;
        }
 
		return -1;
	}
	
	/**
	  * Computes normal for an intersection point on a sphere
	  */
	const glm::vec3 computeNormal(const Ray& r, const float& t) {
		return normal;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, RayTracerState& state) {
		return effect->rayTrace(ray, t, normal, state);
	}

protected:
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 normal;
};

#endif	