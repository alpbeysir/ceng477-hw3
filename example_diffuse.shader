#shader vertex
#version 330 compatibility

vec3 I = vec3(0.75, 0.75, 0.75);

vec3 ks = vec3(0.8, 0.8, 0.8);

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;

uniform vec3 u_LightPosition;
uniform mat4 u_Transformation;
uniform vec3 u_Color;
uniform mat4 u_Prerot;

void main(void)
{
	vec4 world_pos = u_Transformation * vec4(inVertex, 1);

	vec4 hw3_light_pos = world_pos + vec4(0, 0, 1, 0);
	vec4 hw3_eye_pos = hw3_light_pos;

	vec3 L = normalize(vec3(hw3_light_pos) - vec3(world_pos));
	vec3 V = normalize(vec3(hw3_eye_pos) - vec3(world_pos));
	vec3 H = normalize(L + V);
	vec3 N = vec3(transpose(inverse(u_Transformation)) * vec4(inNormal, 0));
	N = normalize(N);
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);

	vec3 diffuseColor = I * u_Color * max(0, NdotL);
	vec3 specularColor = I * ks * pow(max(0, NdotH), 20);

	gl_FrontColor = vec4(diffuseColor + specularColor, 1);
	gl_Position = gl_ModelViewProjectionMatrix * world_pos;
}


#shader fragment
#version 330 compatibility

void main(void)
{
	gl_FragColor = gl_Color; 
}
