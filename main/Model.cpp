#include "Model.hpp"
#include <string>

#define POSITIONS 0
#define NORMALS 1
#define MAT_INDEXES 2


RenderData Model::setupRendering_for_MMMesh(MultiMaterialMesh const& mesh)
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

    data.VBO.emplace_back();
    glGenBuffers(1, &data.VBO.at(MAT_INDEXES));
    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(MAT_INDEXES));
    glBufferData(GL_ARRAY_BUFFER, mesh.materialIndexes.size() * sizeof(int), mesh.materialIndexes.data(), GL_STATIC_DRAW);

    // VAO
    glGenVertexArrays(1, &data.VAO);
    glBindVertexArray(data.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(POSITIONS));
    glVertexAttribPointer(
        POSITIONS,	// location = 0 in vertex shader
        3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
        0,	// stride = 0 no padding
        (GLvoid*)0	// data starts at offset 0 in VBO
    );
    glEnableVertexAttribArray(POSITIONS);

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(NORMALS));
    glVertexAttribPointer(
        NORMALS,	// location = 0 in vertex shader
        3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
        0,	// stride = 0 no padding
        (GLvoid*)0	// data starts at offset 0 in VBO
    );
    glEnableVertexAttribArray(NORMALS);

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(MAT_INDEXES));
    glVertexAttribIPointer(
		MAT_INDEXES,	// location = 2 in vertex shader
		1, GL_INT, 0,	// 1 for material index
		(GLvoid*)0	// data starts at offset 0 in VBO
	);
    glEnableVertexAttribArray(MAT_INDEXES);


    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return data;
}

RenderData Model::setupRendering_for_SMesh(SimpleMesh const& mesh)
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

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(POSITIONS));
    glVertexAttribPointer(
        POSITIONS,	// location = 0 in vertex shader
        3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
        0,	// stride = 0 no padding
        (GLvoid*)0	// data starts at offset 0 in VBO
    );
    glEnableVertexAttribArray(POSITIONS);

    glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(NORMALS));
    glVertexAttribPointer(
        NORMALS,	// location = 0 in vertex shader
        3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
        0,	// stride = 0 no padding
        (GLvoid*)0	// data starts at offset 0 in VBO
    );
    glEnableVertexAttribArray(NORMALS);

    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return data;
}

void Model::renderMMMesh(Vec3f cameraPosition, Mat44f world2projection, Mat44f model2world, Shader& shader)
{
    glUseProgram(shader.data.shaderProgram);
    
    glUniformMatrix4fv(
        0,
        1, GL_TRUE, world2projection.v
    );

    GLuint loc;

    
    // material properties
    for (int i = 0; i < materials.size(); i++) {
        glUniform3f(3 + i * 6, materials.at(i).ambient.x, materials.at(i).ambient.y, materials.at(i).ambient.z);
        glUniform3f(4 + i * 6, materials.at(i).diffuse.x, materials.at(i).diffuse.y, materials.at(i).diffuse.z);
        glUniform3f(5 + i * 6, materials.at(i).specular.x, materials.at(i).specular.y, materials.at(i).specular.z);
        glUniform3f(6 + i * 6, materials.at(i).emission.x, materials.at(i).emission.y, materials.at(i).emission.z);
        glUniform1f(7 + i * 6, materials.at(i).shininess);
        glUniform1f(8 + i * 6, materials.at(i).alpha);
    }

    glUniformMatrix4fv(
        1,
        1, GL_TRUE, model2world.v
    );

    glUniform3f(2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    // Lights
    Vec3f lightPosition = { 0.f, 200.f, 0.f };
    Vec3f ambientLight = { 0.2f, 0.2f, 0.2f };
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

    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::renderSMesh(Vec3f cameraPosition, Mat44f world2projection, Mat44f model2world, Shader& shader)
{
    glUseProgram(shader.data.shaderProgram);

    glUniformMatrix4fv(
        0,
        1, GL_TRUE, world2projection.v
    );

    GLuint loc;

    // material properties
    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Ambient");
    glUniform3f(loc, materials.at(0).ambient.x, materials.at(0).ambient.y, materials.at(0).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Diffuse");
    glUniform3f(loc, materials.at(0).diffuse.x, materials.at(0).diffuse.y, materials.at(0).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Specular");
    glUniform3f(loc, materials.at(0).specular.x, materials.at(0).specular.y, materials.at(0).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Emission");
    glUniform3f(loc, materials.at(0).emission.x, materials.at(0).emission.y, materials.at(0).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Shininess");
    glUniform1f(loc, materials.at(0).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material.Alpha");
    glUniform1f(loc, materials.at(0).alpha);

    glUniformMatrix4fv(
        1,
        1, GL_TRUE, model2world.v
    );

    glUniform3f(2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    // Lights
    Vec3f lightPosition = { 0.f, 200.f, 0.f };
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

    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}