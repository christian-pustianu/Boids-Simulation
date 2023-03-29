#include "cone.hpp"

SimpleMesh make_cone(std::size_t subdivs, Vec3f color, Mat44f transformMatrix)
{
	std::vector<Vertex> vertices;

	Mat33f const N = mat44_to_mat33(transpose(invert(transformMatrix)));

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	for (std::size_t i = 0; i < subdivs; ++i)
	{
		float const angle = (i + 1) / float(subdivs) * 2.f * 3.1415926f;

		float y = std::cos(angle);
		float z = std::sin(angle);

		// One triangle (3 vertices) create one segment of the cone's shell.
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, prevY, prevZ }, Vec3f{ 0.5f, prevY/2, prevZ/2 }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, y, z }, Vec3f{ 0.5f, y/2, z/2 }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 1.f, 0.f, 0.f }, Vec3f{ 0.5f, (prevY + y)/4, (prevZ + z)/4 }, Vec2f{} });

		// One triangle (3 vertices) create one segment of the cone's cap.
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, prevY, prevZ }, Vec3f{ -1.f, 0.f, 0.f }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, 0.f, 0.f }, Vec3f{ -1.f, 0.f, 0.f }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, y, z }, Vec3f{ -1.f, 0.f, 0.f }, Vec2f{} });

		prevY = y;
		prevZ = z;
	}
	for (auto& v : vertices)
	{
		Vec4f p4{ v.positions.x, v.positions.y, v.positions.z, 1.f };
		Vec4f t = transformMatrix * p4;
		t /= t.w;

		v.positions = Vec3f{ t.x, t.y, t.z };

		v.normals = N * v.normals;
	}

	Material material;
	material.ambient = color;
	SimpleMesh mesh(vertices, material);
	return mesh;
}

