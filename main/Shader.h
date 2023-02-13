#pragma once

#include <glad.h>

#include <fstream>


class Shader
{
private:
	const GLchar* vertexShaderSource = "";
	const GLchar* fragmentShaderSource = "";
	
	GLchar* readShader(const char* sourcePath);
	
	GLuint setupVertexShader();
	GLuint setupFragmentShader();
	
public:
	// Container struct for shaders.
	struct ShaderData
	{
		GLuint shaderProgram;
		GLuint vertexShader;
		GLuint fragmentShader;
		bool success;

		void cleanup() {
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(shaderProgram);
		}
	};
	
	ShaderData data;
	
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
		data.cleanup();
	};

	ShaderData setupShaderProgram();
};
