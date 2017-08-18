#ifndef MYSHADER_H_
#define MYSHADER_H_

#include <iostream>
#include <fstream>    //从文件中读取着色器源代码
#include <sstream>
#include <string>

#include <gl/glew.h>  //这是为了包含所有必要的opengl头文件

class MyShader
{
public:
	//着色器程序对象

	GLuint shader_program;
	//构造函数和析构函数，构造函数接受两个参数
	//第一个是顶点着色器的文件路径，第二个是片元着色器的文件路径
	MyShader(const GLchar *vert_path, const GLchar *frag_path);
	~MyShader() {}

	//该函数用于使用着色器程序
	void Use()
	{
		glUseProgram(shader_program);
	}
};

#endif