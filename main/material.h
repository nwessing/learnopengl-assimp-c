#pragma once
#include "cglm/cglm.h"

typedef struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} Material;

Material mat_gold = {
	.ambient = { 0.24725f, 0.1995f, 0.0745f },
	.diffuse = { 0.75164f, 0.60648f, 0.22648f },
	.specular = { 0.628281f, 0.555802f, 0.366065f },
	.shininess = 52
};

Material mat_black_rubber = {
	.ambient = { 0.02f, 0.02f, 0.02f },
	.diffuse = { 0.01f, 0.01f, 0.01f },
	.specular = { 0.4f, 0.4f, 0.4f },
	.shininess = 10
};

Material mat_cyan_plastic = {
	.ambient = { 0.0f, 0.1f, 0.06f },
	.diffuse = { 0.0f, 0.50980392f, 0.50980392f },
	.specular = { 0.50196078f, 0.50196078f, 0.50196078f },
	.shininess = 32
};
