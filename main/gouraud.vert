#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;

out vec3 LightColor;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	vec3 fragPos = vec3(view * model * vec4(aPos, 1.0));
	vec3 normal = mat3(transpose(inverse(view * model))) * aNormal;
	vec3 lightPos = vec3(view * vec4(lightPos, 1.0));

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    int shininess = 256;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    LightColor = specular + ambient + diffuse;
}