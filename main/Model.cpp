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

    for (GLuint i = 0; i < 2; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, data.VBO.at(i));
        glVertexAttribPointer(
            i,	// location = 0 in vertex shader
            3, GL_FLOAT, GL_FALSE, // 3 for positions/normals/
            0,	// stride = 0 no padding
            (GLvoid*)0	// data starts at offset 0 in VBO
        );
        glEnableVertexAttribArray(i);
    }

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

    for (GLuint i = 0; i < 2; i++) {
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
    
    //// material properties
    //for (int i = 0; i < materials.size(); i++) {
    //    char uniformName[30];

    //    sprintf(uniformName, "material[%d].Ambient", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform3f(loc, materials.at(i).ambient.x, materials.at(i).ambient.y, materials.at(i).ambient.z);

    //    sprintf(uniformName, "material[%d].Diffuse", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform3f(loc, materials.at(i).diffuse.x, materials.at(i).diffuse.y, materials.at(i).diffuse.z);

    //    sprintf(uniformName, "material[%d].Specular", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform3f(loc, materials.at(i).specular.x, materials.at(i).specular.y, materials.at(i).specular.z);

    //    sprintf(uniformName, "material[%d].Emission", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform3f(loc, materials.at(i).emission.x, materials.at(i).emission.y, materials.at(i).emission.z);

    //    sprintf(uniformName, "material[%d].Shininess", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform1f(loc, materials.at(i).shininess);

    //    sprintf(uniformName, "material[%d].Alpha", i);
    //    loc = glGetUniformLocation(shader.data.shaderProgram, uniformName);
    //    glUniform1f(loc, materials.at(i).alpha);
    //}

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[0].Ambient");
    glUniform3f(loc, materials.at(0).ambient.x, materials.at(0).ambient.y, materials.at(0).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[0].Diffuse");
    glUniform3f(loc, materials.at(0).diffuse.x, materials.at(0).diffuse.y, materials.at(0).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[0].Specular");
    glUniform3f(loc, materials.at(0).specular.x, materials.at(0).specular.y, materials.at(0).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[0].Emission");
    glUniform3f(loc, materials.at(0).emission.x, materials.at(0).emission.y, materials.at(0).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[0].Shininess");
    glUniform1f(loc, materials.at(0).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Alpha");
    glUniform1f(loc, materials.at(1).alpha);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Ambient");
    glUniform3f(loc, materials.at(1).ambient.x, materials.at(1).ambient.y, materials.at(1).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Diffuse");
    glUniform3f(loc, materials.at(1).diffuse.x, materials.at(1).diffuse.y, materials.at(1).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Specular");
    glUniform3f(loc, materials.at(1).specular.x, materials.at(1).specular.y, materials.at(1).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Emission");
    glUniform3f(loc, materials.at(1).emission.x, materials.at(1).emission.y, materials.at(1).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Shininess");
    glUniform1f(loc, materials.at(1).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[1].Alpha");
    glUniform1f(loc, materials.at(1).alpha);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Ambient");
    glUniform3f(loc, materials.at(2).ambient.x, materials.at(2).ambient.y, materials.at(2).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Diffuse");
    glUniform3f(loc, materials.at(2).diffuse.x, materials.at(2).diffuse.y, materials.at(2).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Specular");
    glUniform3f(loc, materials.at(2).specular.x, materials.at(2).specular.y, materials.at(2).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Emission");
    glUniform3f(loc, materials.at(2).emission.x, materials.at(2).emission.y, materials.at(2).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Shininess");
    glUniform1f(loc, materials.at(2).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[2].Alpha");
    glUniform1f(loc, materials.at(2).alpha);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Ambient");
    glUniform3f(loc, materials.at(3).ambient.x, materials.at(3).ambient.y, materials.at(3).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Diffuse");
    glUniform3f(loc, materials.at(3).diffuse.x, materials.at(3).diffuse.y, materials.at(3).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Specular");
    glUniform3f(loc, materials.at(3).specular.x, materials.at(3).specular.y, materials.at(3).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Emission");
    glUniform3f(loc, materials.at(3).emission.x, materials.at(3).emission.y, materials.at(3).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Shininess");
    glUniform1f(loc, materials.at(3).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[3].Alpha");
    glUniform1f(loc, materials.at(3).alpha);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Ambient");
    glUniform3f(loc, materials.at(4).ambient.x, materials.at(4).ambient.y, materials.at(4).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Diffuse");
    glUniform3f(loc, materials.at(4).diffuse.x, materials.at(4).diffuse.y, materials.at(4).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Specular");
    glUniform3f(loc, materials.at(4).specular.x, materials.at(4).specular.y, materials.at(4).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Emission");
    glUniform3f(loc, materials.at(4).emission.x, materials.at(4).emission.y, materials.at(4).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Shininess");
    glUniform1f(loc, materials.at(4).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[4].Alpha");
    glUniform1f(loc, materials.at(4).alpha);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Ambient");
    glUniform3f(loc, materials.at(5).ambient.x, materials.at(5).ambient.y, materials.at(5).ambient.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Diffuse");
    glUniform3f(loc, materials.at(5).diffuse.x, materials.at(5).diffuse.y, materials.at(5).diffuse.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Specular");
    glUniform3f(loc, materials.at(5).specular.x, materials.at(5).specular.y, materials.at(5).specular.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Emission");
    glUniform3f(loc, materials.at(5).emission.x, materials.at(5).emission.y, materials.at(5).emission.z);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Shininess");
    glUniform1f(loc, materials.at(5).shininess);

    loc = glGetUniformLocation(shader.data.shaderProgram, "material[5].Alpha");
    glUniform1f(loc, materials.at(5).alpha);


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