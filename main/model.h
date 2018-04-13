#pragma once
#include "mesh.h"

typedef struct Model {
	Mesh *meshes;
	u32 numMeshes;
	char *directory;
} Model;

Model loadModel(char *path);
void model_draw(Model *model, ShaderProgram program);
