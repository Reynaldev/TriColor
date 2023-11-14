#version 330 core
in vec3 vColor;

out vec4 fragColor;

uniform vec3 uColor;

void main()
{
	fragColor = vec4(vColor + uColor, 1.0);
}