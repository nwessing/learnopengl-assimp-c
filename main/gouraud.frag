#version 330 core
in vec3 LightColor;

out vec4 FragColor;

uniform vec3 objectColor;

void main()
{
    FragColor = vec4(LightColor * objectColor, 1.0);
}
