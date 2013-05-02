#ifndef SCENEOBJECTEFFECT_HPP__
#define SCENEOBJECTEFFECT_HPP__

#include "Ray.hpp"
#include "RayTracerState.hpp"

/**
  * Abstract class that defines what it means to be an effect for a scene object
  */
class SceneObjectEffect {
public:
	/**
	  * This function "shades" an intersection point between a scene object
	  * and a ray. It can also fire new rays etc.
	  */
	virtual glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) = 0;
private:
};

/**
  * The color effect simply colors any point with the same color
  */
class ColorEffect : public SceneObjectEffect {
public:
	ColorEffect(glm::vec3 color) {
		this->color = color;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		return color;
	}

private:
	glm::vec3 color;
};

/**
  * The phong effect simply uses phong shading to color the intersection point
  */
class PhongEffect : public SceneObjectEffect {
public:
	PhongEffect(glm::vec3 light_pos=glm::vec3(0.0),
				glm::vec3 light_diff=glm::vec3(0.5),
				glm::vec3 light_spec=glm::vec3(0.5)) {
		this->light_pos = light_pos;
		this->light_diff = light_diff;
		this->light_spec = light_spec;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 out_color(0.0f);

		throw std::runtime_error("PhongEffect::rayTrace(...) not implemented yet");

		return out_color;
	}
private:
	glm::vec3 light_pos; //Light position
	glm::vec3 light_diff; //Light diffuse component
	glm::vec3 light_spec; //Light specular component
};

#endif