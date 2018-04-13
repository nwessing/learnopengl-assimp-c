#pragma once
#include <cglm/cglm.h>

typedef struct Scene {
	vec3 clearColor;
	vec3 pointLightColors[4];
} Scene;

Scene desertScene = {
	.clearColor = { 0.75f, 0.52f, 0.3f },
	.pointLightColors = {
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f }
	}
};