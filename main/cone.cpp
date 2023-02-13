#include "cone.hpp"

Mesh make_cone( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	std::vector<Vertex> vertices;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	for (std::size_t i = 0; i < aSubdivs; ++i)
	{
		float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;

		float y = std::cos(angle);
		float z = std::sin(angle);

		// One triangle (3 pos) create one segment of the cone’s shell.
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, prevY, prevZ }, Vec3f{ 0.f, prevY, prevZ }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 0.f, y, z }, Vec3f{ 0.f, y, z }, Vec2f{} });
		vertices.emplace_back(Vertex{ Vec3f{ 1.f, 0.f, 0.f }, Vec3f{ 0.f, 0.f, 0.f }, Vec2f{} });

		prevY = y;
		prevZ = z;
	}
	if (aCapped) {
		for (std::size_t i = 0; i < aSubdivs; ++i)
		{
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;

			float y = std::cos(angle);
			float z = std::sin(angle);

			// One triangle (3 pos) create one segment of the cone’s cap.
			vertices.emplace_back(Vertex{ Vec3f{ 0.f, prevY, prevZ }, Vec3f{ 0.f, prevY, prevZ }, Vec2f{} });
			vertices.emplace_back(Vertex{ Vec3f{ 0.f, 0.f, 0.f }, Vec3f{ 0.f, 0.f, 0.f }, Vec2f{} });
			vertices.emplace_back(Vertex{ Vec3f{ 0.f, y, z }, Vec3f{ 0.f, y, z }, Vec2f{} });

			prevY = y;
			prevZ = z;
		}
	}
	for (auto& v : vertices)
	{
		Vec4f p4{ v.positions.x, v.positions.y, v.positions.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		printf("t: %f %f %f\n", t.x, t.y, t.z);

		v.positions = Vec3f{ t.x, t.y, t.z };
	}

	Material material;
	material.ambient = aColor;
	Mesh mesh(vertices, material);

	for (auto& v : mesh.vertices)
	{
		printf("v: %f %f %f", v.positions.x, v.positions.y, v.positions.z);
	}
	return mesh;
}

