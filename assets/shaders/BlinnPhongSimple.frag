#version 430

struct Material
{
        vec3 Ambient; // k_a
        vec3 Diffuse; // k_d
        vec3 Specular; // k_s
        vec3 Emission; // I_e
        float Shininess; // a'
		float Alpha; // alpha (transparency)
};

struct Light
{
		vec3 Position; // P_light
		vec3 Ambient; // I_a
		vec3 Color; // I_l
		float Strength;
};


// Input data
in vec3 v2fWorldPosition;
in vec3 v2fNormal;

// Uniform data
uniform Light light;
layout( location = 2 ) uniform vec3 camera;
layout(location = 3) uniform Material material;

// Output data
layout( location = 0 ) out vec4 oColor;


void main()
{
	// distance between point light and current vertex
	float dist = length( light.Position - v2fWorldPosition);
	dist = dist * dist;

	// light strength dependent on distance
	vec3 light_color = light.Color * light.Strength / dist;
	
	vec3 L = normalize( light.Position - v2fWorldPosition); // L^
	vec3 N = normalize( v2fNormal ); // N^
	vec3 V = normalize( camera - v2fWorldPosition); // V^
	vec3 H = normalize( L + V ); // H^
	
	float NdotL = max( 0.0, dot( N, L ) );
	float HdotN = pow(max( 0.0, dot( H, N )), material.Shininess);
	
	// Corrected Blinn-Phong
	// I_p = k_a * I_a + k_d * I_l/pi * (N^*L^)_+ + (a' + 2)/8 k_s * I_l * (H^*N^)_+^a' + I_e
	oColor = vec4(material.Ambient * light.Ambient + 
			 material.Diffuse * light_color / 3.1415926 * NdotL + 
			 (material.Shininess + 2) / 8 * material.Specular * light_color * HdotN + 
			 material.Emission, material.Alpha);

}