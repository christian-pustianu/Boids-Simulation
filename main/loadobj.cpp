#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../include/stb_image.h"

SimpleMesh load_wavefront_obj_to_simplemesh(char const* aPath) {
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw std::printf("Unable to load OBJ file '%s': %s", aPath, result.error.code.message().c_str());

	rapidobj::Triangulate(result);

	std::vector<Vertex> vertices;
	Material material;

	for(auto const& shape : result.shapes)
	{
		for(std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
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
		}
	}

	// Find the face index
	auto const& mat = result.materials[0];
	if (!mat.ambient.empty())
		material.ambient = Vec3f{ mat.ambient[0], mat.ambient[1], mat.ambient[2] };

	if (!mat.diffuse.empty())
		material.diffuse = Vec3f{ mat.diffuse[0], mat.diffuse[1], mat.diffuse[2] };

	if (!mat.specular.empty())
		material.specular = Vec3f{ mat.specular[0], mat.specular[1], mat.specular[2] };

	if (!mat.emission.empty())
		material.emission = Vec3f{ mat.emission[0], mat.emission[1], mat.emission[2] };

	if (mat.shininess)
		material.shininess = mat.shininess;

	if (mat.dissolve)
		material.alpha = mat.dissolve;

	return SimpleMesh(vertices, material);
}

MultiMaterialMesh load_wavefront_obj_to_MMmesh(char const* aPath) {
	// Ask rapidobj to load the requested file
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw std::printf("Unable to load OBJ file '%s': %s", aPath, result.error.code.message().c_str());

	rapidobj::Triangulate(result);

	std::vector<Vertex> vertices;
	std::vector<int> materialIndexes;
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

	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
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

			materialIndexes.push_back(shape.mesh.material_ids[i/3]);
		}
	}

	return MultiMaterialMesh(vertices, materials, materialIndexes);
}