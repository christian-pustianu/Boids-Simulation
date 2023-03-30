#include "Terrain.hpp"

//SimpleMesh make_terrain(const char* heightmap, Mat44f transformMatrix)
//{
//    std::vector<Vertex> vertices;
//    float yScale = 64.0f / 256.0f, yShift = 16.0f;  // apply a scale+shift to the height data
//    Material material;
//    int width, height, nChannels;
//    unsigned char* data = stbi_load("assets/heightmap.png",
//        &width, &height, &nChannels,
//        0);
//
//    for (unsigned int i = 0; i < height; i++)
//    {
//        for (unsigned int j = 0; j < width; j++)
//        {
//            // retrieve texel for (i,j) tex coord
//            unsigned char* texel = data + (j + width * i) * nChannels;
//            // raw height at coordinate
//            unsigned char y = texel[0];
//
//            // vertex
//            vertices.emplace_back(Vertex{ Vec3f{ -height / 2.0f + i, (int)y * yScale - yShift, -width / 2.0f + j}, 
//                Vec3f{}, Vec2f{} });
//        }
//    }
//    stbi_image_free(data);
//    return SimpleMesh(vertices, material);
//}
