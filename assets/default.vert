#version 430

// Input data
layout ( location = 0 ) in vec3 iPosition;
layout ( location = 1 ) in vec3 iNormal;

// Uniform data
layout( location = 0 ) uniform mat4 uWorld2projection;
layout( location = 1 ) uniform mat4 uModel2world;

out vec3 v2fWorldPosition;
out vec3 v2fNormal;

void main()
{
	mat3 normalMatrix = mat3(transpose(inverse(uModel2world)));
	v2fNormal = normalize(normalMatrix * iNormal);
	vec4 worldPosition = uModel2world * vec4( iPosition.xyz, 1.0 );
	v2fWorldPosition = worldPosition.xyz;

	// Copy position to the built-in gl Position attribute
	gl_Position = uWorld2projection * worldPosition;
}
