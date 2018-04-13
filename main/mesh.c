#include <glad/glad.h>
#include <string.h>
#include <stdio.h>
#include "mesh.h"

void mesh_setup(Mesh *mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);
	glGenBuffers(1, &mesh->EBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

	glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->numIndices * sizeof(u32), mesh->indices, GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// tex coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);
}

void mesh_draw(Mesh *mesh, ShaderProgram program)
{
	u32 iDiffuse = 1;
	u32 iSpecular = 1;

	for (u32 i = 0; i < mesh->numTextures; i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		Texture *texture = &mesh->textures[i];
		int number = 0;
		if (strcmp("texture_diffuse", texture->type) == 0) {
			number = iDiffuse;
			++iDiffuse;
		}
		else if (strcmp("texture_specular", texture->type) == 0) {
			number = iSpecular;
			++iSpecular;
		}

		i8 materialUniformName[256];
		sprintf(materialUniformName, "material.%s%i", texture->type, number);
		glGetUniformLocation(program, materialUniformName);
		shaderProgramSetInt(program, materialUniformName, i);
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(mesh->VAO);
	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}