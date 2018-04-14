#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdio.h>
#include "types.h"
#include "model.h"
#include "texture.h"

typedef struct aiScene aiScene;
typedef struct aiNode aiNode;
typedef struct aiMaterial aiMaterial;
typedef struct aiMesh aiMesh;
typedef struct aiFace aiFace;

u32 model_countMeshes(const aiNode *node);
void model_processNode(Model *model, const aiNode *node, const aiScene *scene, u32 *meshIndex);
Mesh model_processMesh(Model *model, const aiMesh *mesh, const aiScene *scene);
Texture *loadMaterialTextures(Model *model, const aiMaterial *mat, i32 type, i8 *typeName, u32 *numTextures);

static Texture *cachedTextures = NULL;
static u32 numCachedTextures = 0;
static u32 sizeCachedTextures = 0;

Model loadModel(char *path)
{
	Model model;
	const aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		printf("ERROR:ASSIMP::%s\n", aiGetErrorString());
	}

	u32 index = strrchr(path, '/') - path;
	model.directory = calloc(index + 1, sizeof(i8));
	strncpy(model.directory, path, index);
	model.directory[index] = '\0';

	u32 numMeshes = model_countMeshes(scene->mRootNode);
	model.meshes = calloc(numMeshes, sizeof(Mesh));
	model.numMeshes = numMeshes;

	u32 meshIndex = 0;
	model_processNode(&model, scene->mRootNode, scene, &meshIndex);
	return model;
}

void model_draw(Model *model, ShaderProgram program)
{
	for (u32 i = 0; i < model->numMeshes; i++) {
		mesh_draw(&model->meshes[i], program);
	}
}

void model_processNode(Model *model, const aiNode *node, const aiScene *scene, u32 *meshIndex)
{
	for (u32 i = 0; i < node->mNumMeshes; i++) {
		const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		model->meshes[*meshIndex] = model_processMesh(model, mesh, scene);
		(*meshIndex)++;
	}

	for (u32 i = 0; i < node->mNumChildren; i++) {
		model_processNode(model, node->mChildren[i], scene, meshIndex);
	}
}

u32 model_countMeshes(const aiNode *node)
{
	u32 result = node->mNumMeshes;

	for (u32 i = 0; i < node->mNumChildren; i++) {
		result += model_countMeshes(node->mChildren[i]);
	}

	return result;
}

Mesh model_processMesh(Model *model, const aiMesh *mesh, const aiScene *scene)
{
	Vertex *vertices = calloc(mesh->mNumVertices, sizeof(Vertex));
	for (u32 i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex = {0};
		vertex.position[0] = mesh->mVertices[i].x;
		vertex.position[1] = mesh->mVertices[i].y;
		vertex.position[2] = mesh->mVertices[i].z;

		vertex.normal[0] = mesh->mNormals[i].x;
		vertex.normal[1] = mesh->mNormals[i].y;
		vertex.normal[2] = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
			vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
		}
		vertices[i] = vertex;
	}

	u32 numIndices = 0;
	for (u32 i = 0; i < mesh->mNumFaces; i++) {
		numIndices += mesh->mFaces[i].mNumIndices;
	}

	u32 *indices = calloc(numIndices, sizeof(u32));
	u32 iIndices = 0;
	for (u32 i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (u32 j = 0; j < face.mNumIndices; j++) {
			indices[iIndices] = face.mIndices[j];
			++iIndices;
		}
	}

	u32 numDiffuseMaps = 0;
	u32 numSpecularMaps = 0;
	Texture *diffuseMaps = NULL;
	Texture *specularMaps = NULL;
	if (scene->mNumMaterials > 0) {
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse", &numDiffuseMaps);
		specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular", &numSpecularMaps);
	}

	Texture *textures = NULL;
	u32 numTextures = numDiffuseMaps + numSpecularMaps;
	if (numTextures > 0) {
		textures = calloc(numTextures, sizeof(Texture));

		if (diffuseMaps != NULL) {
			memcpy(textures, diffuseMaps, sizeof(Texture) * numDiffuseMaps);
			free(diffuseMaps);
		}

		if (specularMaps != NULL) {
			memcpy(&textures[numDiffuseMaps], specularMaps, sizeof(Texture) * numSpecularMaps);
			free(specularMaps);
		}
	}

	Mesh result = {0};
	result.vertices = vertices;
	result.numVertices = mesh->mNumVertices;
	result.indices = indices;
	result.numIndices = numIndices;
	result.textures = textures;
	result.numTextures = numTextures;

	mesh_setup(&result);
	return result;
}

Texture *loadMaterialTextures(Model *model, const aiMaterial *mat, i32 type, i8 *typeName, u32 *numTextures)
{
	*numTextures = aiGetMaterialTextureCount(mat, type);
	if (*numTextures == 0) {
		return NULL;
	}

	if (cachedTextures == NULL) {
		sizeCachedTextures = 2;
		cachedTextures = calloc(sizeCachedTextures, sizeof(Texture));
	}

	Texture *textures = calloc(*numTextures, sizeof(Texture));

	for (u32 i = 0; i < *numTextures; i++) {
		aiString str;
		aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

		Texture *cachedTexture = NULL;
		for (u32 iCache = 0; iCache < numCachedTextures; ++iCache) {
			if (strcmp(str.data, cachedTextures[iCache].path.data) == 0) {
				cachedTexture = &cachedTextures[iCache];
			}
		}

		if (cachedTexture == NULL) {
			u32 dirLength = strlen(model->directory);
			i8 *texturePath = calloc(str.length + dirLength + 2, sizeof(i8));
			strcpy(texturePath, model->directory);
			strcat(texturePath, "/");
			strcat(texturePath, str.data);

			Texture texture = {0};
			texture.id = initTexture(texturePath);
			texture.type = typeName;
			texture.path = str;
			textures[i] = texture;
			free(texturePath);

			if (numCachedTextures == sizeCachedTextures) {
				sizeCachedTextures = sizeCachedTextures * 2;
				cachedTextures = realloc(cachedTextures, sizeCachedTextures * sizeof(Texture));
			}

			cachedTextures[numCachedTextures] = texture;
			++numCachedTextures;
		}
		else {
			textures[i] = *cachedTexture;
		}
	}

	return textures;
}