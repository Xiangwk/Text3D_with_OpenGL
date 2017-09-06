//使用FreeType2解析字体文件，将轮廓线的绘制点信息取出

#ifndef FREETYPE_FONT_H_
#define FREETYPE_FONT_H_

#include <iostream>
#include <fstream>
#include <vector>

//glm
#include <glm\glm.hpp>

//FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include "Primitive.h"
#include "Geometry.h"

namespace FreeType
{
	//这个类用来存放outline分解之后的顶点数组和索引数组
	//以及每条contour对应的绘制单元
	struct Char3DInfo
	{
		//成员变量
		Vec3Array vertices;                      //当前轮廓线的顶点数组
		ElementArray current_indices;            //当前轮廓线的索引数组
		std::vector<ElementArray> contour_list;  //轮廓线列表，每条轮廓线需要对应不同的索引数组，但是共享同一个顶点数组
		glm::vec3 previous;                      //当前绘制点的前一个顶点
		GLuint num_steps;                        //贝塞尔曲线绘制采样步数
		GLfloat coord_scale;                     //因为FreeType中采用1/64像素单位描述字形参量，这个值为1/64

		//默认构造函数
		Char3DInfo() : num_steps(10), coord_scale(1.0 / 64.0) {}

		//完成当前轮廓线的数据采集
		void completeCurrentGeom()
		{
			if (!vertices.empty() && !current_indices.empty())
				contour_list.push_back(current_indices);
			current_indices.clear();
		}
		//获取解析完成的字形几何体，每一个字形对应一个几何体
		Geometry get()
		{
			completeCurrentGeom();
			return Geometry(vertices, contour_list);
		}

		//向当前顶点数组和索引数组中添加顶点及其索引
		void addVertex(glm::vec3 pos)
		{
			previous = pos;
			pos *= coord_scale;
			//避免连续添加相同的顶点
			if (!vertices.empty() && vertices.back() == pos)
				return;
			//对于一条contour中非连续的重复点，无需重复添加
			if (!current_indices.empty() && vertices[current_indices[0]] == pos)
				current_indices.push_back(current_indices[0]);
			else
			{
				current_indices.push_back(vertices.size());
				vertices.push_back(pos);
			}

		}
		//开始绘制新的轮廓线
		void moveTo(const glm::vec2 &pos)
		{
			completeCurrentGeom();
			addVertex(glm::vec3(pos.x, pos.y, 0));
		}
		//绘制直线段
		void lineTo(const glm::vec2 &pos)
		{
			addVertex(glm::vec3(pos.x, pos.y, 0));
		}
		//绘制二次贝塞尔曲线
		void conicTo(const glm::vec2 &control, const glm::vec2 &pos)
		{
			glm::vec3 p0 = previous;
			glm::vec3 p1 = glm::vec3(control.x, control.y, 0);
			glm::vec3 p2 = glm::vec3(pos.x, pos.y, 0);

			GLfloat dt = 1.0 / num_steps;
			GLfloat u = 0;
			for (unsigned int i = 0; i <= num_steps; ++i)
			{
				GLfloat w = 1;   //w可能只是一个调整参数
				GLfloat bs = 1.0 / ((1 - u)*(1 - u) + 2 * (1 - u)*u*w + u*u);
				//注意：这里直接copy的OSG里面的公式，bs貌似只是一个调整参数，原贝塞尔曲线方程里没有这个bs（B-Spline？）
				//而且当w的值为1时，bs永远等于1
				glm::vec3 p = (p0*((1 - u)*(1 - u)) + p1*(2 * (1 - u)*u*w) + p2*(u*u)) * bs;
				addVertex(p);

				u += dt;
			}
		}
		//绘制三次贝塞尔曲线
		void cubicTo(const glm::vec2 &control1, const glm::vec2 &control2, const glm::vec2 &pos)
		{
			glm::vec3 p0 = previous;
			glm::vec3 p1 = glm::vec3(control1.x, control1.y, 0);
			glm::vec3 p2 = glm::vec3(control2.x, control2.y, 0);
			glm::vec3 p3 = glm::vec3(pos.x, pos.y, 0);

			GLfloat cx = 3 * (p1.x - p0.x);
			GLfloat bx = 3 * (p2.x - p1.x) - cx;
			GLfloat ax = p3.x - p0.x - cx - bx;
			GLfloat cy = 3 * (p1.y - p0.y);
			GLfloat by = 3 * (p2.y - p1.y) - cy;
			GLfloat ay = p3.y - p0.y - cy - by;

			GLfloat dt = 1.0 / num_steps;
			GLfloat u = 0;
			for (unsigned int i = 0; i <= num_steps; ++i)
			{
				glm::vec3 p = glm::vec3(ax*u*u*u + bx*u*u + cx*u + p0.x, ay*u*u*u + by*u*u + cy*u + p0.y, 0);
				addVertex(p);

				u += dt;
			}
		}
	};  //Char3DInfo定义结束

	//这四个函数用于FT_Outline_Decompose中的FT_Funcs参数
	//他们用来回调Char3DInfo中真正执行轮廓分解的四个同名函数
	//返回值0代表处理成功
	int moveTo(const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->moveTo(glm::vec2(to->x, to->y));
		return 0;
	}
	int lineTo(const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->lineTo(glm::vec2(to->x, to->y));
		return 0;
	}
	int conicTo(const FT_Vector* control, const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->conicTo(glm::vec2(control->x, control->y), glm::vec2(to->x, to->y));
		return 0;
	}
	int cubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
	{
		Char3DInfo* char3d = (Char3DInfo*)user;
		char3d->cubicTo(
			glm::vec2(control1->x, control1->y),
			glm::vec2(control2->x, control2->y),
			glm::vec2(to->x, to->y));
		return 0;
	}
}   //命名空间FreeType关闭



class FreeTypeFont
{
public:
	FreeTypeFont(wchar_t code, GLchar *font_file);
	~FreeTypeFont()
	{
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	Geometry getGlyph3D();
	GLuint advanceX() const { return AdvanceX; }

private:
	wchar_t charcode;    //字符编码
	FT_Library ft;
	FT_Face face;
	GLuint AdvanceX;
	FreeType::Char3DInfo char3d;
};

FreeTypeFont::FreeTypeFont(wchar_t code, char *font_file)
{
	//初始化FreeType
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	//新建一个face
	if (FT_New_Face(ft, font_file, 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	//设置文字大小
	FT_Set_Pixel_Sizes(face, 0, 24);

	charcode = code;
	//加载文字以获取步进宽度（不加载文字是无法获取步进宽度的）
	FT_Load_Char(face, charcode, FT_LOAD_RENDER);
	AdvanceX = (face->glyph->advance.x) >> 6;
}

Geometry FreeTypeFont::getGlyph3D()
{
	//加载字形
	FT_Error error = FT_Load_Char(face, charcode, FT_LOAD_DEFAULT);
	if (error)
		std::cout << "FT_Load_Char(...) error 0x" << std::hex << error << std::dec << std::endl;
	if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
		std::cout << "FreeTypeFont3D::getGlyph : not a vector font" << std::endl;

	FT_Outline outline = face->glyph->outline;
	FT_Outline_Funcs funcs;
	funcs.conic_to = (FT_Outline_ConicToFunc)&FreeType::conicTo;
	funcs.line_to  = (FT_Outline_LineToFunc )&FreeType::lineTo;
	funcs.cubic_to = (FT_Outline_CubicToFunc)&FreeType::cubicTo;
	funcs.move_to  = (FT_Outline_MoveToFunc )&FreeType::moveTo;
	funcs.shift = 0;
	funcs.delta = 0;

	//分解字形轮廓线，注册回调函数
	FT_Error _error = FT_Outline_Decompose(&outline, &funcs, &char3d);
	if (_error)
		std::cout << "FreeTypeFont3D::getGlyph : - outline decompose failed ..." << std::endl;
	
	//返回字形几何体
	return char3d.get();
}

#endif