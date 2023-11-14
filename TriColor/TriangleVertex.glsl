#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

uniform vec3 uPos;

void main()
{
	gl_Position = vec4(aPos + uPos, 1.0);
	vColor = aColor;
}