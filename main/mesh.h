#pragma once
#include <cglm/cglm.h>
#include <assimp/scene.h>
#include "types.h"
#include "shader.h"

typedef struct aiString aiString;
typedef float vec2[2];

typedef struct Vertex {
	vec3 position;
	vec3 normal;
	vec2 texCoords;
} Vertex;

typedef struct Texture {
	u32 id;
	char* type;
	aiString path;
} Texture;

typedef struct Mesh {
	Vertex *vertices;
	u32 numVertices;
	u32 *indices;
	u32 numIndices;
	Texture *textures;
	u32 numTextures;
	u32 VAO;
	u32 VBO;
	u32 EBO;
} Mesh;

void mesh_draw(Mesh *mesh, ShaderProgram program);
void mesh_setup(Mesh *mesh);