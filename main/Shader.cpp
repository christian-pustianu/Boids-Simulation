#include "Shader.hpp"


// load shader from file into a string
GLchar* Shader::readShader(const char* sourcePath) {
    FILE* file = fopen(sourcePath, "rb");
    if (!file) {
        printf("Error: unable to open shader source file\n");
        return {};
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Allocate a buffer to hold the file contents
    char* buffer = new char[size + 1];
    if (!buffer) {
        printf("Error: unable to allocate memory for shader source\n");
        fclose(file);
        return {};
    }

    // Read the file contents into the buffer
    size_t read_size = fread(buffer, 1, size, file);
    if (read_size != size) {
        printf("Error: unable to read shader source file\n");
        delete[] buffer;
        fclose(file);
        return {};
    }

    fclose(file);
    buffer[size] = '\0';
    return buffer;
}

unsigned int Shader::setupVertexShader()
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::printf("ERROR:VERTEX_SHADER:COMPILATION_FAILED\n%s\n", infoLog);
        return 0;
    }

    return vertexShader;
}

unsigned int Shader::setupFragmentShader()
{
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::printf("ERROR:FRAGMENT_SHADER:COMPILATION_FAILED\n%s\n", infoLog);
        return 0;
    }

    return fragmentShader;
}


Shader::ShaderData Shader::setupShaderProgram()
{
    ShaderData result = {};
    result.shaderProgram = glCreateProgram();
    result.vertexShader = setupVertexShader();
    result.fragmentShader = setupFragmentShader();

    if (result.vertexShader == 0 || result.fragmentShader == 0)
    {
        return {};
    }

    glAttachShader(result.shaderProgram, result.vertexShader);
    glAttachShader(result.shaderProgram, result.fragmentShader);
    glLinkProgram(result.shaderProgram);

    int  success;
    char infoLog[512];
    glGetProgramiv(result.shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(result.shaderProgram, 512, NULL, infoLog);
        std::printf("ERROR:SHADER_PROGRAM:COMPILATION_FAILED\n%s\n", infoLog);
        cleanup();
        return {};
    }

    result.success = true;
    return result;
}