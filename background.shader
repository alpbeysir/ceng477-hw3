#shader vertex
#version 330 core

layout(location = 4) in vec2 position;
layout(location = 5) in vec3 vertColor;

out vec3 v_vertColor;

void main()
{
    vec4 temp = vec4(position.x, position.y, 0.5f, 1.0f);
    gl_Position = temp;

    v_vertColor = vertColor;
}

#shader fragment
#version 330 core

in vec3 v_vertColor;

void main()
{
	vec4 texColor = vec4(v_vertColor.x, v_vertColor.y, v_vertColor.z, 1.0f);
    gl_FragColor = texColor;
}
