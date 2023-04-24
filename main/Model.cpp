#include "Model.hpp"
#include <string>

#define POSITIONS 0
#define NORMALS 1
#define MAT_INDEXES 2


void Model::setupRendering()
{
    std::vector<Vec3f> positions, normals;
    for (Vertex const& v : this->vertices) {
        positions.emplace_back(v.positions);
        normals.emplace_back(v.normals);
    }

    // VBO
    this->VBO.emplace_back();
    glGenBuffers(1, &this->VBO.at(POSITIONS));
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(POSITIONS));
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vec3f), positions.data(), GL_STATIC_DRAW);

    this->VBO.emplace_back();
    glGenBuffers(1, &this->VBO.at(NORMALS));
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(NORMALS));
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3f), normals.data(), GL_STATIC_DRAW);

    // VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(POSITIONS));
    glVertexAttribPointer(
        POSITIONS,	// location in .vert
        3, GL_FLOAT, GL_FALSE, // 3 floats for positions
        0,	// no padding
        (GLvoid*)0	// offset 0 in VBO
    );
    glEnableVertexAttribArray(POSITIONS);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(NORMALS));
    glVertexAttribPointer(
        NORMALS,	// location in .vert
        3, GL_FLOAT, GL_FALSE, // 3 floats for normals
        0,	// no padding
        (GLvoid*)0	// offset 0 in VBO
    );
    glEnableVertexAttribArray(NORMALS);

    if (materialIndexes.size() > 0) {
        this->VBO.emplace_back();
        glGenBuffers(1, &this->VBO.at(MAT_INDEXES));
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(MAT_INDEXES));
        glBufferData(GL_ARRAY_BUFFER, materialIndexes.size() * sizeof(int), materialIndexes.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, this->VBO.at(MAT_INDEXES));
        glVertexAttribIPointer(
		    MAT_INDEXES,	// location in .vert
		    1, GL_INT,	// 1 int for material index
            0,	// no padding
		    (GLvoid*)0	// offset 0 in VBO
	    );
        glEnableVertexAttribArray(MAT_INDEXES);
    }

    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::render(Vec3f cameraPosition, Light light, Mat44f world2projection, Mat44f givenModel2world, GLuint shaderProgs[])
{
    GLuint shaderProg;
    if (materials.size() == 1)
        shaderProg = shaderProgs[0];
    else
        shaderProg = shaderProgs[1];
    
    glUseProgram(shaderProg);
    
    glUniformMatrix4fv(
        0,
        1, GL_TRUE, world2projection.v
    );


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
        1, GL_TRUE, givenModel2world.v
    );

    glUniform3f(2, cameraPosition.x, cameraPosition.y, cameraPosition.z);

    GLuint loc;
    loc = glGetUniformLocation(shaderProg, "light.Position");
    glUniform3f(loc, light.position.x, light.position.y, light.position.z);

    loc = glGetUniformLocation(shaderProg, "light.Ambient");
    glUniform3f(loc, light.ambient.x, light.ambient.y, light.ambient.z);

    loc = glGetUniformLocation(shaderProg, "light.Color");
    glUniform3f(loc, light.color.x, light.color.y, light.color.z);

    loc = glGetUniformLocation(shaderProg, "light.Strength");
    glUniform1f(loc, light.strength);

    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());

    // Reset state
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
