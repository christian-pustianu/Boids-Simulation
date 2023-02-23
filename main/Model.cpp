#include "Model.hpp"

#define POSITIONS 0
#define NORMALS 1


RenderData Model::setupRendering(Mesh const& mesh)
{
    RenderData data;

    std::vector<Vec3f> positions, normals;
    for (Vertex const& v : mesh.vertices) {
        positions.emplace_back(v.positions);
        normals.emplace_back(v.normals);
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

    for (GLuint i = 0; i < data.VBO.size(); i++) {
        glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(i));
        glVertexAttribPointer(
            i,	// location = 0 in vertex shader
            3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
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

void Model::render(Shader shader)
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

    glUniformMatrix4fv(
        1,
        1, GL_TRUE, this->model2world.v
    );

    // Lights
    Vec3f lightPosition = { 0.f, 50.f, 0.f };
    Vec3f ambientLight = { 0.05f, 0.05f, 0.05f };
    Vec3f lightColor = { 1.f, 1.f, 1.f };
    float lightStrength = 10000.f;

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Position");
    glUniform3f(loc, lightPosition.x, lightPosition.y, lightPosition.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Ambient");
    glUniform3f(loc, ambientLight.x, ambientLight.y, ambientLight.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Color");
    glUniform3f(loc, lightColor.x, lightColor.y, lightColor.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "light.Strength");
    glUniform1f(loc, lightStrength);

    glBindVertexArray(this->data.VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);
}