#pragma once

#include <glad.h>

#include <fstream>


/**
* @brief Representation of a shader.
*/
class Shader
{
private:
	// Container struct for shaders.
	struct ShaderData
	{
		GLuint shaderProgram;
		GLuint vertexShader;
		GLuint fragmentShader;
		bool success;
	};

	const GLchar* vertexShaderSource = "";
	const GLchar* fragmentShaderSource = "";
	
	/**
	* @brief Reads the shader source code from a file into a string.
	* 
	* @param sourcePath - Path to the shader source file.
	*
	* @return string of the shader source.
	*/
	GLchar* readShader(const char* sourcePath);
	
	/**
	* @brief Compiles the vertex shader.
	*
	* @return ID of the vertex shader.
	*/
	GLuint setupVertexShader();

	/**
	* @brief Compiles the fragment shader.
	*
	* @return ID of the fragment shader.
	*/
	GLuint setupFragmentShader();

	/**
	* @brief Links the vertex and fragment shaders to a new shader program.
	*
	* @return ShaderData struct containing the shader program and the vertex and fragment shader IDs.
	*/
	ShaderData setupShaderProgram();

	/**
	* @brief Deletes ShaderData associated with current object.
	*
	* @return void.
	*/
	void cleanup() {
		glDeleteShader(this->data.vertexShader);
		glDeleteShader(this->data.fragmentShader);
		glDeleteProgram(this->data.shaderProgram);
	}
	
public:
	ShaderData data;
	
	/**
	* @brief Constructor for a shader. Reads the shader source code from the given files and compiles them.
	*/
	Shader(const char* vertSource, const char* fragSource) {
		vertexShaderSource = readShader(vertSource);
		fragmentShaderSource = readShader(fragSource);
		data = setupShaderProgram();
		if (!data.success)
		{
			throw std::exception("Shader compilation failed.");
		}
	};

	~Shader() {
		cleanup();
	};

};
