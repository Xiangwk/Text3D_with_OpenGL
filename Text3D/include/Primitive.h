//Primitive是绘制字形的最小单元，一般字形中每一个contour使用一个Geometry

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include <iostream>
#include <vector>

#include <gl\glew.h>

#include <glm\glm.hpp>

#include "MyShader.h"

//顶点数组和索引数组
typedef std::vector<glm::vec3> Vec3Array;
typedef std::vector<GLuint> ElementArray;

//顶点结构
struct Vertex
{
	glm::vec3 position;  //位置坐标
	glm::vec3 normal;    //法向量
	//glm::vec2 texcoord;  //纹理坐标
};

class Primitive
{
private:
	GLuint VAO, VBO, EBO;
	GLenum mode;     //绘制图元类型
	void SetupVAO();
public:
	std::vector<Vertex> vertices;
	ElementArray indices;
	Primitive() = default;
	Primitive(GLenum m, Vec3Array vec3s, ElementArray eles, Vec3Array norms);
	~Primitive() {}
	//绘制
	void draw(MyShader shader);

	//清除缓存对象
	void deleteBuffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

#endif