#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../include/stb_image.h"

Model load_wavefront_obj(char const* aPath) {
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw std::printf("Unable to load OBJ file '%s': %s", aPath, result.error.code.message().c_str());

	rapidobj::Triangulate(result);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> materialIndexes;
	std::vector<Material> materials;

	for (auto mat : result.materials) {
		Material material;

		material.ambient = Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] };
		material.diffuse = Vec3f{ mat.diffuse[0], mat.diffuse[1], mat.diffuse[2] };
		material.specular = Vec3f{ mat.specular[0], mat.specular[1], mat.specular[2] };
		material.emission = Vec3f{ mat.emission[0], mat.emission[1], mat.emission[2] };
		material.shininess = mat.shininess;
		material.alpha = mat.dissolve;

		materials.push_back(material);
	}

	for(auto const& shape : result.shapes) {
		for(std::size_t i = 0; i < shape.mesh.indices.size(); ++i) {
			auto const& idx = shape.mesh.indices[i];

			vertices.emplace_back(Vertex{
			Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
			},
			Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
			}, Vec2f{} });

			if(materials.size() > 0)
				materialIndexes.push_back(shape.mesh.material_ids[i/3]);
		}						
	}

	if (materials.size() > 0)
		return Model(vertices, materials, materialIndexes);
	else
		return Model(vertices, materials.at(0));
}
