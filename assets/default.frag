#version 430

struct Material
{
        vec3 Ambient; // K_a
        vec3 Diffuse; // K_d
        vec3 Specular; // K_s
        vec3 Emission; // I_e
        float Shininess; // a'
		float Alpha; // alpha (transparency)
};

struct Light
{
		vec3 Position; // P_light
		vec3 Ambient; // I_a
		vec3 Diffuse; // I_d
		vec3 Specular; // I_s
};

// Input data
in vec3 v2fPosition;
in vec3 v2fNormal;

// Uniform data
uniform Material material;
uniform Light light;
uniform vec3 camera;

// Output data
layout( location = 0 ) out vec4 oColor;


void main()
{
	// get distance between light and fragment and make 
	// light strength depend on the distance
	float dist = length( light.Position - v2fPosition );
	vec3 uLightDiffuse = light.Diffuse / (dist * dist);
	vec3 uLightSpecular = light.Specular / (dist * dist);
	
	// I_p = K_a*I_a + K_d*I_d(N^.L^)_+ + K_s*I_s(H^.N^)_+^a' + I_e
	vec3 L = normalize( light.Position - v2fPosition ); // L^
	vec3 N = normalize( v2fNormal ); // N^
	vec3 V = normalize( camera - v2fPosition ); // V^
	vec3 H = normalize( L + V ); // H^

	float nDotL = max( 0.0, dot( N, L ) );
	float hDotN = pow(max( 0.0, dot( H, N )), material.Shininess);

//	oColor = vec4(material.Ambient * light.Ambient + 
//			 material.Diffuse * light.Diffuse * nDotL + 
//			 material.Specular * light.Specular * hDotN + 
//			 material.Emission, material.Alpha);

	oColor = vec4(N, material.Alpha);
//	oColor = N;

}