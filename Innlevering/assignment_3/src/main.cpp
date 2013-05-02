#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "RayTracer.h"
#include "Sphere.hpp"
#include "CubeMap.hpp"
#include "Timer.h"

/**
 * Simple program that starts our raytracer
 */
int main(int argc, char *argv[]) {
	try {
		RayTracer* rt;
		Timer t;
		rt = new RayTracer(800, 600);
		
		std::shared_ptr<SceneObjectEffect> color(new ColorEffect(glm::vec3(0.0, 1.0, 0.0)));
		std::shared_ptr<SceneObjectEffect> phong(new PhongEffect(glm::vec3(0.0, 0.0, 10.0)));
		
		std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-3.0f, 0.0f, 6.0f), 2.0f, color));
		rt->addSceneObject(s1);
		std::shared_ptr<SceneObject> s2(new Sphere(glm::vec3(3.0f, 0.0f, 3.0f), 2.0f, color));
		rt->addSceneObject(s2);
		std::shared_ptr<SceneObject> s3(new Sphere(glm::vec3(0.0f, 3.0f, 9.0f), 2.0f, color));
		rt->addSceneObject(s3);
				
		t.restart();
		rt->render();
		double elapsed = t.elapsed();
		std::cout << "Computed in " << elapsed << " seconds" <<  std::endl;
		rt->save("test", "bmp"); //We want to write out bmp's to get proper bit-maps (jpeg encoding is lossy)

		delete rt;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		return -1;
	}
	return 0;
}
