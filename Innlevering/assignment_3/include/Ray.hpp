#ifndef _RAY_HPP__
#define _RAY_HPP__

#include <glm/glm.hpp>

/**
  * The ray class holds the state information of a ray in our ray-tracer:
  * point of origin and direction
  */
class Ray {
public:
	Ray(glm::vec3 origin, glm::vec3 direction) {
		this->origin = origin;
		this->direction = direction;
		depth = 0;
	}

	/**
	  * Returns the origin of the ray
	  */
	inline const glm::vec3& getOrigin() const { return origin; }

	/**
	  * Returns the direction of the ray
	  */
	inline const glm::vec3& getDirection() const { return direction; }

	/**
	  * Spanws a new ray from this ray originating from getOrigin() + t*getDirection() 
	  * going in the direction of d
	  */
	inline Ray spawn(float t, glm::vec3 d) const {
		Ray r(getOrigin()+t*getDirection(), d);
		r.depth = this->depth + 1;
		return r;
	}

	/**
	  * Tests whether or not this ray should be raytraced further
	  */
	inline bool isValid() const {
		return (depth <= max_depth);
	}

	/**
	  * Invalidate ray, saying it should not be raytraced further
	  */
	inline void invalidate() {
		this->depth = max_depth;
	}

private:
	friend class RayTracer;

	static const unsigned int max_depth = 7;
	unsigned int depth;
	glm::vec3 origin;
	glm::vec3 direction;
};

#endif