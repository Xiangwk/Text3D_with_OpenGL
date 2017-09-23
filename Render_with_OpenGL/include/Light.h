#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

//attenuation struct
struct Attenuation
{
	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};
//cutoff struct
struct Cutoff
{
	//cutoff angle
	GLfloat innerCutoff;
	GLfloat outerCutoff;
};

class DirLight
{
public:
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	void setUniform(GLuint id, GLuint offset);
};

class PointLight
{
public:
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	//the attenuation factor
	Attenuation atten;

	void setUniform(GLuint id, GLuint offset);
};

class SpotLight
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	//the attenuation factor
	Attenuation atten;
	//cutoff angle
	Cutoff cutoff;

	void setUniform(GLuint id, GLuint offset);
};

void DirLight::setUniform(GLuint id, GLuint offset)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset +  0, 16, glm::value_ptr(direction));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 16, glm::value_ptr(ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 16, glm::value_ptr(diffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, 16, glm::value_ptr(specular));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void PointLight::setUniform(GLuint id, GLuint offset)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 0, 16, glm::value_ptr(position));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 16, glm::value_ptr(ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 16, glm::value_ptr(diffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, 16, glm::value_ptr(specular));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 64, 4, &atten.constant);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 68, 4, &atten.linear);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 72, 4, &atten.quadratic);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SpotLight::setUniform(GLuint id, GLuint offset)
{
	glBindBuffer(GL_UNIFORM_BUFFER, id);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 0, 16, glm::value_ptr(position));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, 16, glm::value_ptr(direction));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, 16, glm::value_ptr(ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, 16, glm::value_ptr(diffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 64, 16, glm::value_ptr(specular));
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 80, 4, &atten.constant);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 84, 4, &atten.linear);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 88, 4, &atten.quadratic);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 96, 4, &cutoff.innerCutoff);
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 100, 4, &cutoff.outerCutoff);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}