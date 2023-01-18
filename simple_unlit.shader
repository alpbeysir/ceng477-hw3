#shader vertex
#version 330 compatibility

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * vec4(position, 1);
}

#shader fragment
#version 330 compatibility

void main()
{
	gl_FragColor = vec4(1, 0, 0, 1);
}
