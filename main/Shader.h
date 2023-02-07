#pragma once

#include <glad.h>

#include <fstream>


class Shader
{
private:
	const char* vertexShaderSource = "";
	const char* fragmentShaderSource = "";
	
	GLchar* readShader(const char* sourcePath);
	
	unsigned int setupVertexShader();
	unsigned int setupFragmentShader();
	
public:
	// Container struct for shaders.
	struct ShaderData
	{
		unsigned int shaderProgram;
		unsigned int vertexShader;
		unsigned int fragmentShader;
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

	ShaderData setupShaderProgram();
};
