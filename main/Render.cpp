#include "Render.h"

#define POSITIONS 0
#define NORMALS 1
//#define TEXCOORDS 2


RenderData Render::setupRendering(Mesh const& mesh)
{
    RenderData data;
    
    std::vector<Vec3f> positions, colors;
    for (Vertex const& v : mesh.vertices) {
        positions.emplace_back(v.positions);
        colors.emplace_back(mesh.material.ambient);
    }
    
    // VBO
    data.VBO.emplace_back();
    glGenBuffers(1, &data.VBO.at(POSITIONS));
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(POSITIONS));
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vec3f), positions.data(), GL_STATIC_DRAW);

    data.VBO.emplace_back();
    glGenBuffers(1, &data.VBO.at(NORMALS));
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(NORMALS));
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vec3f), colors.data(), GL_STATIC_DRAW);

    //data.VBO.emplace_back();
    //glGenBuffers(1, &data.VBO.at(NORMALS));
    //glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(NORMALS));
    //glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vec3f), &mesh.vertices.data()->normals, GL_STATIC_DRAW);

    // VAO
    glGenVertexArrays(1, &data.VAO);
    glBindVertexArray(data.VAO);

    int j = 3;
    //for (GLuint i = 0; i < data.VBO.size(); i++) {
    for (GLuint i = 0; i < 2; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(i));
        //if (i == TEXCOORDS) j = 2;
        glVertexAttribPointer(
            i,	// location = 0 in vertex shader
            j, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
            0,	// stride = 0 no padding
            (GLvoid*)0	// data starts at offset 0 in VBO
        );
        glEnableVertexAttribArray(i);
    }

    // Reset state
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return data;
}

void Render::render(Shader shader)
{
    glUseProgram(shader.data.shaderProgram);

    //glUniform3fv(
    //    0,
    //    1,
    //    &this->mat.alpha);

    glBindVertexArray(this->data.VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
}