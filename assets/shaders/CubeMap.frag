#version 430
in vec3 v2fTexCoords;

layout( binding = 0 ) uniform samplerCube cubemap;

out vec4 oColor;

void main()
{    
	oColor = texture(cubemap, v2fTexCoords);
}