#version 430

// Input attributes
// These should match the outputs from the vertex shader.
in vec3 v2fColor;

// Uniform data

// Fragment shader outputs
layout( location = 0 ) out vec3 oColor;

void main()
{
	oColor = v2fColor;
}
