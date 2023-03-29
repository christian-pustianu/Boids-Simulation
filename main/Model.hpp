#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "Mesh.hpp"

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"

#include <cstdlib>
#include <ctime>

struct RenderData {
	std::vector<GLuint> VBO;
	GLuint VAO = 0;
};

class Model {
private:
	void cleanup() {
		for (GLuint vbo : data.VBO)
			glDeleteBuffers(1, &vbo);

		glDeleteVertexArrays(1, &data.VAO);
	};

public:
	RenderData data;
	std::vector<Material> materials;
	std::vector<int> materialIndexes;
	std::size_t vertexCount;

	Mat44f model2world = Identity44f;

	//Model(Mesh& mesh) {
	//	if (SimpleMesh* sm = dynamic_cast<SimpleMesh*>(&mesh)) {

	//	}
	//}

	Model(SimpleMesh mesh) {
		data = setupRendering(mesh);
		vertexCount = mesh.vertices.size();
		materials.push_back(mesh.material);
		materialIndexes = std::vector<int>(vertexCount, 0);
	};

	Model(MultiMaterialMesh mesh) {
		materialIndexes = mesh.materialIndexes;
		data = setupRendering(mesh);
		vertexCount = mesh.vertices.size();
		materials = mesh.materials;
	};

	// TODO: TexturedMesh
	//Model(TexturedMesh mesh) {
	//	data = setupRendering(mesh);
	//	vertexCount = mesh.vertices.size();
	//	materials.push_back(Material());
	//};

	~Model() {
		cleanup();
	};
	
	RenderData setupRendering(Mesh const&);
	void render(Vec3f, Mat44f, Mat44f, Shader&);
};
