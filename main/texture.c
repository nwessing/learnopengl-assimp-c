#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>

#include "texture.h"
#include "stb_image.h"

TextureImage initTexture(char *imageName)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	u8 *data = stbi_load(imageName, &width, &height, &nrChannels, 0);
	if (!data) {
		printf("Failed to load texture.\n");
		return 0;
	}

	GLenum format = GL_RGB;
	if (nrChannels == 1) {
		format = GL_RED;
	}
	else if (nrChannels == 3) {
		format = GL_RGB;
	}
	else if (nrChannels == 4) {
		format = GL_RGBA;
	}

	TextureImage texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_image_free(data);
	return texture;
}