#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "RayTracer.h"
#include "Sphere.hpp"
#include "Triangle.h"
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
		std::shared_ptr<SceneObjectEffect> phong(new PhongEffect(glm::vec3(0.3, 0.3, 0.3), glm::vec3(0.0, 0.0, 10.0)));
		std::shared_ptr<SceneObjectEffect> fresnel(new FresnelEffect());
		std::shared_ptr<SceneObjectEffect> reflect(new ReflectSteelEffect());

		std::shared_ptr<SceneObject> s1(new Sphere(glm::vec3(-3.0f, 0.0f, 6.0f), 2.0f, fresnel));
		rt->addSceneObject(s1);
		std::shared_ptr<SceneObject> s2(new Sphere(glm::vec3(3.0f, 0.0f, 3.0f), 2.0f, phong));
		rt->addSceneObject(s2);
		std::shared_ptr<SceneObject> s3(new Sphere(glm::vec3(0.0f, 3.0f, 9.0f), 2.0f, reflect));
		rt->addSceneObject(s3);
		std::shared_ptr<SceneObject> s4(new Triangle(glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), reflect));
		rt->addSceneObject(s4);

		std::shared_ptr<SceneObject> cube_map(new CubeMap("cubemaps/SaintLazarusChurch3/posx.jpg", "cubemaps/SaintLazarusChurch3/negx.jpg",
			"cubemaps/SaintLazarusChurch3/posy.jpg", "cubemaps/SaintLazarusChurch3/negy.jpg",
			"cubemaps/SaintLazarusChurch3/posz.jpg", "cubemaps/SaintLazarusChurch3/negz.jpg"));
		rt->addSceneObject(cube_map);
				
		t.restart();
		rt->render();
		double elapsed = t.elapsed();
		std::cout << "Computed in " << elapsed << " seconds" <<  std::endl;
		rt->save("test", "bmp"); //We want to write out bmp's to get proper bit-maps (jpeg encoding is lossy)

		delete rt;
	} catch (std::exception &e) {
		std::string err = e.what();
		std::cout << err.c_str() << std::endl;
		//std::system("pause");
		return -1;
	}
	return 0;
}
