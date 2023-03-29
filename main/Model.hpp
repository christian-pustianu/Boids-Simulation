#pragma once

#include <glad.h>
#include "Shader.hpp"
#include "Mesh.hpp"

#include "../math/mat44.hpp"
#include "../math/vec3.hpp"

#include <cstdlib>
#include <ctime>
#include <string>

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
	std::size_t vertexCount;
	std::vector<std::string> uniformNames;

	Mat44f model2world = Identity44f;

	Model(SimpleMesh mesh) {
		data = setupRendering_for_SMesh(mesh);
		vertexCount = mesh.vertices.size();
		materials.push_back(mesh.material);
	};

	Model(MultiMaterialMesh mesh) {
		data = setupRendering_for_MMMesh(mesh);
		vertexCount = mesh.vertices.size();
		materials = mesh.materials;
		for (int i = 0; i < materials.size(); i++) {
			std::string name = "material[" + std::to_string(i) + "].Ambient";
			uniformNames.push_back(name);
			name = "material[" + std::to_string(i) + "].Diffuse";
			uniformNames.push_back(name);
			name = "material[" + std::to_string(i) + "].Specular";
			uniformNames.push_back(name);
			name = "material[" + std::to_string(i) + "].Emission";
			uniformNames.push_back(name);
			name = "material[" + std::to_string(i) + "].Shininess";
			uniformNames.push_back(name);
			name = "material[" + std::to_string(i) + "].Alpha";
			uniformNames.push_back(name);
		}
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
	
	RenderData setupRendering_for_SMesh(SimpleMesh const& mesh);
	RenderData setupRendering_for_MMMesh(MultiMaterialMesh const& mesh);
	//RenderData setupRendering(Mesh const&);
	void renderMMMesh(Vec3f, Mat44f, Mat44f, Shader&);
	void renderSMesh(Vec3f, Mat44f, Mat44f, Shader&);
};
