#version 430

// Input data
layout ( location = 0 ) in vec3 iPosition;
layout ( location = 1 ) in vec3 iColor;

// Uniform data
layout( location = 0 ) uniform mat4 uProjCameraWorld;

// Output data
out vec3 v2fColor; // v2f = vertex to fragment

void main()
{
	// Copy input color to the output color attribute
	v2fColor = iColor;

	// Copy position to the built-in gl Position attribute
	gl_Position = uProjCameraWorld * vec4( iPosition.xyz, 1.0 );
}
