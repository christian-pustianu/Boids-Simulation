#include "Render.h"

#define POSITIONS 0
#define NORMALS 1
//#define TEXCOORDS 2


RenderData Render::setupRendering(Mesh const& mesh)
{
    RenderData data;

    std::vector<Vec3f> positions, normals;
    for (Vertex const& v : mesh.vertices) {
        positions.emplace_back(v.positions);
        normals.emplace_back(v.normals);
        printf("normals: %f %f %f\n", v.normals.x, v.normals.y, v.normals.z);
    }

    // VBO
    data.VBO.emplace_back();
    glGenBuffers(1, &data.VBO.at(POSITIONS));
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(POSITIONS));
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vec3f), positions.data(), GL_STATIC_DRAW);

    data.VBO.emplace_back();
    glGenBuffers(1, &data.VBO.at(NORMALS));
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(NORMALS));
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3f), normals.data(), GL_STATIC_DRAW);

    // VAO
    glGenVertexArrays(1, &data.VAO);
    glBindVertexArray(data.VAO);

    int j = 3;
    for (GLuint i = 0; i < data.VBO.size(); i++) {
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

    // material properties
    GLuint loc = glGetUniformLocation(shader.data.shaderProgram, "material.Ambient");
    glUniform3f(loc, mat.ambient.x, mat.ambient.y, mat.ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Diffuse");
    glUniform3f(loc, mat.diffuse.x, mat.diffuse.y, mat.diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Specular");
    glUniform3f(loc, mat.specular.x, mat.specular.y, mat.specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Emission");
    glUniform3f(loc, mat.emission.x, mat.emission.y, mat.emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Shininess");
    glUniform1f(loc, mat.shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Alpha");
    glUniform1f(loc, mat.alpha);

    // Lights
    Vec3f LightPos = { 0.f, 1.2f, 0.f };
    Vec3f ambientLight = { 0.03f, 0.03f, 0.03f };
    Vec3f diffuseLight = { 4.5f, 4.5f, 3.f };
    Vec3f specularLight = { 0.5f, 0.5f, 0.5f };

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Postion");
    glUniform3fv(loc, 1, &LightPos.x);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Ambient");
    glUniform3fv(loc, 1, &ambientLight.x);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Diffuse");
    glUniform3fv(loc, 1, &diffuseLight.x);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Specular");
    glUniform3fv(loc, 1, &specularLight.x);

    // Camera
    Vec3f camera = { 0.f, 0.f, -5.f };
    loc = glGetUniformLocation(shader.data.shaderProgram, "camera");
    glUniform3fv(loc, 1, &camera.x);


    glBindVertexArray(this->data.VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
}