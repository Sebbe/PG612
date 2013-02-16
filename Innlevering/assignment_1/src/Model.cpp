#include "Model.h"

#include "GameException.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Model::Model(std::string filename, bool invert) {
	std::vector<float> vertexNormal_data;
	std::vector<glm::vec3> vertex;
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	scene = aiImportFile(filename.c_str(), aiProcessPreset_TargetRealtime_Quality);// | aiProcess_FlipWindingOrder);
	if (!scene) {
		std::string log = "Unable to load mesh from ";
		log.append(filename);
		THROW_EXCEPTION(log);
	}

	/**
	  * FIXME: Alter loadRecursive, so that it also loads normal data
	  */
	//Load the model recursively into data
	loadRecursive(root, invert, vertexNormal_data, vertex, scene, scene->mRootNode);
	
	//Set the transformation matrix for the root node
	//These are hard-coded constants for the stanford bunny model.
	max_dim.x = -std::numeric_limits<float>::max();
	min_dim.x = std::numeric_limits<float>::max();

	max_dim.y = -std::numeric_limits<float>::max();
	min_dim.y = std::numeric_limits<float>::max();

	max_dim.z = -std::numeric_limits<float>::max();
	min_dim.z = std::numeric_limits<float>::max();

	/*
	for(int i = 0; i < vertex.size(); i += 6) {
		//std::cout << "max: " << vertex[i].x << " " << vertex[i].y << " " << vertex[i].z << std::endl;
		if(max_dim.x < vertexNormal_data[i]) max_dim.x = vertexNormal_data[i];
		if(max_dim.y < vertexNormal_data[i+1]) max_dim.y = vertexNormal_data[i+1];
		if(max_dim.z < vertexNormal_data[i+2]) max_dim.z = vertexNormal_data[i+2];
		if(min_dim.x > vertexNormal_data[i]) min_dim.x = vertexNormal_data[i];
		if(min_dim.y > vertexNormal_data[i+1]) min_dim.y = vertexNormal_data[i+1];
		if(min_dim.z > vertexNormal_data[i+2]) min_dim.z = vertexNormal_data[i+2];
	}*/
	
	for(int i = 0; i < vertex.size(); i++) {
		//std::cout << "max: " << vertex[i].x << " " << vertex[i].y << " " << vertex[i].z << std::endl;
		if(max_dim.x < vertex[i].x) max_dim.x = vertex[i].x;
		if(max_dim.y < vertex[i].y) max_dim.y = vertex[i].y;
		if(max_dim.z < vertex[i].z) max_dim.z = vertex[i].z;
		if(min_dim.x > vertex[i].x) min_dim.x = vertex[i].x;
		if(min_dim.y > vertex[i].y) min_dim.y = vertex[i].y;
		if(min_dim.z > vertex[i].z) min_dim.z = vertex[i].z;
	}
	std::cout << "max: " << max_dim.x << " " << max_dim.y << " " << max_dim.z << std::endl;
	std::cout << "min: " << min_dim.x << " " << min_dim.y << " " << min_dim.z << std::endl;

	glm::vec3 distance = max_dim - min_dim;
	std::cout << "distance: " << distance.x << " " << distance.y << " " << distance.z << std::endl;
	float scaleFactor;

	if(distance.x > distance.y)
		scaleFactor = distance.x;
	else
		scaleFactor = distance.y;

	if(distance.z > scaleFactor)
		scaleFactor = distance.z;
	
	glm::vec3 scale;
	scale.x = 1.0f / scaleFactor;
	scale.y = 1.0f / scaleFactor;
	scale.z = 1.0f / scaleFactor;
	std::cout << "scalefactor: " << scaleFactor << " scale: " << scale.x << " " << scale.y << " " << scale.z << std::endl;
	glm::vec3 center = (max_dim + min_dim);
	center.x /= -2;
	center.y /= -2;
	center.z /= -2;
	std::cout << "bounding: " << center.x << " " << center.y << " " << center.z << std::endl;
	root.transform = glm::scale(root.transform, glm::vec3(scale));
	root.transform = glm::translate(root.transform, center);

	n_vertices = vertexNormal_data.size();

	//Create the VBOs from the data.
	if (fmod(static_cast<float>(n_vertices), 3.0f) < 0.000001f) {
		interleavedVBO.reset(new GLUtils::VBO(vertexNormal_data.data(), n_vertices*sizeof(float)));
	}
	else
		THROW_EXCEPTION("The number of vertices in the mesh is wrong");
}

Model::~Model() {

}

void Model::loadRecursive(MeshPart& part, bool invert,
	std::vector<float>& vertexNormal_data, std::vector<glm::vec3>& vertex, const aiScene* scene, const aiNode* node) {
	//update transform matrix. notice that we also transpose it
	aiMatrix4x4 m = node->mTransformation;
	for (int j=0; j<4; ++j)
		for (int i=0; i<4; ++i)
			part.transform[j][i] = m[i][j];

	// draw all meshes assigned to this node
	for (unsigned int n=0; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];

		//apply_material(scene->mMaterials[mesh->mMaterialIndex]);

		part.first = vertexNormal_data.size()/6;
		part.count = mesh->mNumFaces*3;

		//Allocate data
		vertexNormal_data.reserve(vertexNormal_data.size() + part.count*3);
		vertex.reserve(vertex.size() + part.count*3);
		//Add the vertices from file
		for (unsigned int t = 0; t < mesh->mNumFaces; ++t) {
			const struct aiFace* face = &mesh->mFaces[t];

			if(face->mNumIndices != 3)
				THROW_EXCEPTION("Only triangle meshes are supported");

			bool hasNormals = mesh->HasNormals();
			for(unsigned int i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				glm::vec3 vert;
				vert.x = mesh->mVertices[index].x;
				vert.y = mesh->mVertices[index].y;
				vert.z = mesh->mVertices[index].z;
				vertex.push_back(vert);
				//vertexter
				vertexNormal_data.push_back(mesh->mVertices[index].x);
				vertexNormal_data.push_back(mesh->mVertices[index].y);
				vertexNormal_data.push_back(mesh->mVertices[index].z);

				//normaler
				vertexNormal_data.push_back(mesh->mNormals[index].x);
				vertexNormal_data.push_back(mesh->mNormals[index].y);
				vertexNormal_data.push_back(mesh->mNormals[index].z);

			}
		}
	}

	// load all children
	for (unsigned int n = 0; n < node->mNumChildren; ++n) {
		part.children.push_back(MeshPart());
		loadRecursive(part.children.back(), invert, vertexNormal_data, vertex, scene, node->mChildren[n]);
	}
}