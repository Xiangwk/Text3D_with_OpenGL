#pragma once

#include <glad\glad.h>

#include <stb_image.h>

#include <iostream>
#include <string>

struct Texture2D
{
	GLuint id;
	std::string type;    //texture's type(diffuse texture or specular texture or others)
	std::string path;    //texture source image's file path(this member is used to identify the texture's source image)

	void loadFromFile(const std::string p);
};

void Texture2D::loadFromFile(const std::string p)
{
	glGenTextures(1, &id);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(p.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		switch (nrComponents)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			break;
		}
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cerr << "Failed to load image from file: " << p << std::endl;
	}
	stbi_image_free(data);
}