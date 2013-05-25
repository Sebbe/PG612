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
	/**
	based on the effect, the light dissapears
	*/

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
	PhongEffect(glm::vec3 color,
				glm::vec3 light_pos=glm::vec3(0.0),
				glm::vec3 light_diff=glm::vec3(1.0),
				glm::vec3 light_spec=glm::vec3(0.7)) {
		this->light_pos = light_pos;
		this->light_diff = light_diff;
		this->light_spec = light_spec;
		this->color = color;
	}

	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 out_color = color;
		ray.invalidate();
		glm::vec3 g_l = glm::normalize(light_pos - (ray.getOrigin() + ray.getDirection() * t));
		glm::vec3 g_v = glm::normalize(-ray.getDirection());


		float diff = glm::max(0.0f, glm::dot(normal, g_l));
		float spec = pow(glm::max(0.0f, glm::dot(normal, glm::normalize(g_v + g_l))), 128.0f);

		out_color = glm::vec3(diff * light_diff * out_color + light_spec * spec);
		//throw std::runtime_error("PhongEffect::rayTrace(...) not implemented yet");

		return out_color;
	}
private:
	glm::vec3 light_pos; //Light position
	glm::vec3 light_diff; //Light diffuse component
	glm::vec3 light_spec; //Light specular component
	glm::vec3 color;
};

class FresnelEffect : public SceneObjectEffect {
public:
	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		const float eta_air = 1.000293f;
		const float eta_carbondioxide = 1.00045f;
		const float eta_water = 1.3330f;
		const float eta_ethanol = 1.361f;
		const float eta_pyrex = 1.470f;
		const float eta_diamond = 2.419f;

		const float eta0 = eta_air;
		const float eta1 = eta_water;

		float eta = eta0/eta1;
		float R0 = pow((eta0-eta1)/(eta0+eta1), 2.0f);

		glm::vec3 n = normal;
		glm::vec3 v = ray.getDirection();

	
		float fresnel;

		glm::vec3 reflect;
		glm::vec3 refract;
		/*
		if(glm::dot(v, n) <= 0) { 
			// enter
			R0 = pow((eta0-eta1)/(eta0+eta1), 2.0f);
			eta = eta0/eta1;

			newRefractDir = glm::refract(v, n, eta);
			newReflectDir = glm::reflect(v, n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
		} else { 
			// exit
			R0 = pow((eta1-eta0)/(eta0+eta1), 2.0f);
			eta = eta1/eta0;

			newRefractDir = glm::refract(v, -n, eta);
			newReflectDir = glm::reflect(v, -n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(newRefractDir, n)), 5.0f);
		}
		
		if(glm::dot(v, n) <= 0) { // enter
			R0 = pow((eta0-eta1)/(eta0+eta1), 2.0f);
			eta = eta0/eta1;
			refract = glm::refract(v, n, eta);
			reflect = glm::reflect(v, n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);
			
		} else { //exit
			R0 = pow((eta1-eta0)/(eta0+eta1), 2.0f);
			eta = eta1/eta0;
			//reflect = glm::reflect(v, n);
			//refract = glm::refract(v, n, eta);
			refract = glm::refract(v, -n, eta);
			reflect = glm::reflect(v, -n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(refract, n)), 5.0f);
		} */
		if(glm::dot(v, n) >= 0) { // exiting
			
			R0 = pow((eta1-eta0)/(eta0+eta1), 2.0f);
			eta = eta1/eta0;
			//fresnel = R0 + (1.0f-R0)*pow((1.0f-glm::dot(v, n)), 5.0f);
			
			//reflect = glm::reflect(v, n);
			//refract = glm::refract(v, n, eta);
			refract = glm::refract(v, -n, eta);
			reflect = glm::reflect(v, -n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(refract, n)), 5.0f);
		} else { //entering
			
			R0 = pow((eta0-eta1)/(eta0+eta1), 2.0f);
			eta = eta0/eta1;
			refract = glm::refract(-v, n, eta);
			reflect = glm::reflect(-v, n);
			fresnel = R0 + (1.0f-R0)*glm::pow((1.0f-glm::dot(-v, n)), 5.0f);

		}

		Ray reflect_ray = ray.spawn(t, reflect, 10);
		Ray refract_ray = ray.spawn(t, refract, 10);

		glm::vec3 reflect1 = state.rayTrace(reflect_ray);
		glm::vec3 refract1 = state.rayTrace(refract_ray);

		refract1 *= glm::vec3(eta, 1.0f, eta);

		//return normal;
		return glm::mix(refract1, reflect1, fresnel);
		//return refract1;
	}
};

class ReflectSteelEffect : public SceneObjectEffect {
public:
	glm::vec3 rayTrace(Ray &ray, const float& t, const glm::vec3& normal, RayTracerState& state) {
		glm::vec3 n = glm::normalize(normal);
		glm::vec3 v = glm::normalize(ray.getDirection());

		glm::vec3 reflect = glm::reflect(v, n);

		Ray reflect_ray = ray.spawn(t, reflect, 10);

		glm::vec3 reflect1 = state.rayTrace(reflect_ray);
		return glm::vec3(reflect1);
	}
};
#endif