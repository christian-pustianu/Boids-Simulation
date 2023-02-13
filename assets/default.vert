#version 430

// Input data
layout ( location = 0 ) in vec3 iPosition;
layout ( location = 1 ) in vec3 iNormal;

// Uniform data
layout( location = 0 ) uniform mat4 uProjCameraWorld;

void main()
{
	// Copy position to the built-in gl Position attribute
	gl_Position = uProjCameraWorld * vec4( iPosition.xyz, 1.0 );
}
