#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <Shader.h>

#include <vector>
#include <string>

#include <Texture2D.h>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	/*Vertex(const glm::vec3 &pos, const glm::vec3 &n, const glm::vec2 &tex = glm::vec2()) :
		position(pos), normal(n), texCoord(tex) {}*/
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture2D> textures;

	GLuint VAO, VBO, EBO;

	explicit Mesh(const std::vector<Vertex> &v, const std::vector<GLuint> &i = {}, const std::vector<Texture2D> &t = {});
	void draw(Shader shader) const;

private:
	void setupVAO();
};

Mesh::Mesh(const std::vector<Vertex> &v, const std::vector<GLuint> &i, const std::vector<Texture2D> &t) :
vertices(v), indices(i), textures(t) 
{
	setupVAO();
}

void Mesh::draw(Shader shader) const
{
	unsigned diffNum = 1, specNum = 1;
	for (size_t i = 0; i < textures.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		int n = 0;
		std::string t = textures[i].type;
		//we use n to judge the texture's name in shader
		//in shader we must named textures as "texture_diffuseN" or "texture_specularN" or others(N is start from 1)
		if (t == "texture_diffuse")
			n = diffNum++;
		else if (t == "texture_specular")
			n = specNum++;
		std::string N = std::to_string(n);
		//don't forget "material.", in shader the sampler of texture is in Material structure
		shader.setUniformInt("material." + t + N, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	if (!indices.empty())
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupVAO()
{
	if (vertices.empty())
	{
		std::cerr << "Vertices load failed!" << std::endl;
		return;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	if (!indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
	}
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}