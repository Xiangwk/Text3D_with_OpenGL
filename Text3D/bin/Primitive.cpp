#include "..\include\Primitive.h"

Primitive::Primitive(GLenum m, Vec3Array vec3s, ElementArray eles, Vec3Array norms)
{
	mode = m;
	for (int i = 0; i < eles.size(); ++i)
		vertices.push_back({ vec3s[eles[i]], norms[i] });
	/*for (int i = 0; i < vec3s.size(); ++i)
	vertices.push_back({ vec3s[i], norms[i] });*/
	indices = eles;

	SetupVAO();
}

void Primitive::SetupVAO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Primitive::draw(MyShader shader)
{
	shader.Use();
	glBindVertexArray(VAO);
	//glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(mode, 0, vertices.size());
	glBindVertexArray(0);
}