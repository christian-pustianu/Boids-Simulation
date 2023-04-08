#version 430
in vec3 v2fTexCoords;

layout( binding = 0 ) uniform samplerCube cubeMap;

out vec4 oColor;

void main()
{    
	oColor = texture(cubeMap, v2fTexCoords);
}