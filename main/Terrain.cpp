#include "Terrain.hpp"

Model make_terrain(const char* heightmap, Vec3f color, Mat44f transformMatrix) {
    std::vector<Vertex> vertices;
    int width, height, nChannels;
    unsigned char* data = stbi_load(heightmap,
        &width, &height, &nChannels,
        0);

    Mat33f const N = mat44_to_mat33(transpose(invert(transformMatrix)));

    //printf("%d %d", width, height); //2726 1756
    //printf("%s", data + (width-2 + width * (height-2)) * nChannels); //0

    for (unsigned int i = 0; i < width-1; i++) {
        for (unsigned int j = 0; j < height-1; j++) {
            //Triangle 1
            // retrieve texel for (i,j) tex coord
            unsigned char* texel = data + (i + width * j) * nChannels;
            // raw height at coordinate
            float y = texel[0];
            // vertices
            Vec3f p0{ -width / 2.0f + i, y, -height / 2.0f + j };
            
            texel = data + (i + width * (j+1)) * nChannels;
            y = texel[0];
            Vec3f p1{ -width / 2.0f + i, y, -height / 2.0f + j + 1 };
            
            texel = data + (i + 1 + width * j) * nChannels;
            y = texel[0];
            Vec3f p2{ -width / 2.0f + i + 1, y, -height / 2.0f + j };
            // normal for triangle 1
            Vec3f u = p1 - p0;
            Vec3f v = p2 - p0;
            Vec3f n = cross(u, v);
            vertices.emplace_back(Vertex{ p0, n, Vec2f{} });
            vertices.emplace_back(Vertex{ p1, n, Vec2f{} });
            vertices.emplace_back(Vertex{ p2, n, Vec2f{} });

            //Triangle 2
            texel = data + (i + 1 + width * (j + 1)) * nChannels;
            y = texel[0];
            Vec3f p3{ -width / 2.0f + i + 1, y, -height / 2.0f + j + 1 };

            u = p3 - p1;
            v = p2 - p1;
            n = cross(u, v);

            vertices.emplace_back(Vertex{ p2, n, Vec2f{} });
            vertices.emplace_back(Vertex{ p1, n, Vec2f{} });
            vertices.emplace_back(Vertex{ p3, n, Vec2f{} });
        }
    }
    stbi_image_free(data);

    for (auto& v : vertices) {
        Vec4f p4{ v.positions.x, v.positions.y, v.positions.z, 1.f };
        Vec4f t = transformMatrix * p4;
        t /= t.w;

        v.positions = Vec3f{ t.x, 10.f*t.y, t.z };

        v.normals = N * v.normals;
    }

    Material material;
    material.ambient = color;
    return Model(vertices, material);
}
