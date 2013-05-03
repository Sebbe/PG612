#ifndef _SPHERE_HPP__
#define _SPHERE_HPP__

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
		float abc = (b*b-4.0f*a*c);

		static int initialized=0;
		if (!initialized) {
			std::cerr << "The Sphere::intersect(...) function is not implemented properly!" << std::endl;
			++initialized;
		}

		//Just test whether or not we hit the ray to display something at least
		if (abc >= 0.0f) {
			float t0 = (-b - sqrt(abc)) / (2*a);
			float t1 = (-b + sqrt(abc)) / (2*a);

			if((t0 * t1) < 0) {
				if(t0 > t1) {
					return t0;
				}
				return t1;
			}

			if(t0 > 0 && t1 > 0) {
				if(t0 > t1) {
					return t1;
				}
				return t0;
			}

			return -1;
		}
		else {
			return -1;
		}
			/**
			vecteur dist = s.pos - r.start; 
		double B = r.dir * dist;
		double D = B*B - dist * dist + s.size * s.size; 
		if (D < 0.0f) 
			return false; 
		double t0 = B - sqrt(D); 
		double t1 = B + sqrt(D);
		bool retvalue = false;  
		if ((t0 > 0.1f) && (t0 < t)) 
		{
			t = (float)t0;
			retvalue = true; 
		} 
		if ((t1 > 0.1f) && (t1 < t)) 
		{
			t = (float)t1; 
			retvalue = true; 
		}
		*/
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