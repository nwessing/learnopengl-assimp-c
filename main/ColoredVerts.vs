# version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float xOffset;

out vec3 ourColor;

void main()
{
	gl_Position = vec4(aPos.x + xOffset, aPos.y * -1.0f, aPos.z, 1.0);
	ourColor = gl_Position.xyz;
}
