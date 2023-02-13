#version 430

// Input attributes
// These should match the outputs from the vertex shader.
in vec3 v2fColor;

struct Material
{
        vec3 Ambient;
        vec3 Diffuse;
        vec3 Specular;
        vec3 Emission;
        float Shininess;
		float Alpha;
};

in Material v2fMaterial;

// Uniform data
uniform Material Mat;

// Fragment shader outputs
layout( location = 0 ) out vec4 oColor;

void main()
{
	oColor = vec4(Mat.Ambient.xyz, Mat.Alpha);
}
