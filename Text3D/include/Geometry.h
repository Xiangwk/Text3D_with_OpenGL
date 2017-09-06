//每一个Geometry对应一个字形
//Geometry中有唯一的顶点数组和多个索引数组
//每个索引数组对应不同的轮廓线

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Primitive.h"
#include "TriangleIndexFunctor.h"

class Geometry
{
public:
	//构造函数
	Geometry() = default;
	Geometry(Vec3Array& va, std::vector<ElementArray>& el)
	{
		_vertices = va;
		_indices = el;
	}
	~Geometry() 
	{
		//for (auto &p : _primitive_list)
			//p.deleteBuffers();
	}

	//获取顶点数组
	Vec3Array* getVertexArray() { return &_vertices; }

	//获取索引列表
	std::vector<ElementArray>& getPrimitiveSetList() { return _indices; }

	//获取索引列表的大小，也就是几何体中轮廓线的数量
	unsigned int getNumPrimitiveSets() { return _indices.size(); }

	//删除指定范围内的索引数组
	bool removePrimitiveSet(unsigned int b, unsigned int numToRemove);

	void addNormalArray(const Vec3Array &normals)
	{
		_normals.push_back(normals);
	}

	Vec3Array getNormalArray();

	ElementArray getIndices();

	//添加索引数组
	void addPrimitiveSet(ElementArray* element) { _indices.push_back(*element); }

	//添加绘制图元类型
	void addMode(GLenum mode) { _mode_list.push_back(mode); }
	//清空图元列表
	void clearModeList() { _mode_list.clear(); }

	//使用一个索引生成函数，用于生成适用于GL_TRIANGLES的索引数组
	void accept(PrimitiveIndexFunctor& functor) const;

	//绘制
	void draw(MyShader shader);

	//输出数据到文件
	void output();

	void updataPrimitives()
	{
		/*if (_primitive_list.empty())
		{
		for (int i = 0; i < _indices.size(); ++i)
		_primitive_list.push_back(Primitive(GL_TRIANGLES, _vertices, _indices[i], _normals[i]));
		}*/
		p = Primitive(GL_TRIANGLES, _vertices, getIndices(), getNormalArray());
	}

private:
	Vec3Array _vertices;                       //顶点数组
	std::vector<ElementArray> _indices;        //索引列表
	std::vector<Vec3Array> _normals;                        //法向量数组
	//这个图元列表是用来存储经由gluTess细分后生成的轮廓图元类型
	//gluTess的细分程序可能生成三种图元类型，分别是GL_TRIANGLES,GL_TRIANGLE_STRIP,GL_TRIANGLE_FAN
	//每种类型对应的索引不同，我们需要在细分后使用我们自己的GL_TRIANGLES索引生成器将索引类型统一
	std::vector<GLenum> _mode_list;            
	//std::vector<Primitive> _primitive_list;    //绘制图元
	Primitive p;
	//更新图元列表，这个函数应该在绘制图元之前调用
	
};

//用来收集生成好的GL_TRIANGLES索引
struct CollectTriangleIndicesFunctor
{
	CollectTriangleIndicesFunctor() {}

	typedef std::vector<unsigned int> Indices;
	Indices _indices;

	void operator() (unsigned int p1, unsigned int p2, unsigned int p3)
	{
		if (p1 == p2 || p2 == p3 || p1 == p3)
		{
			return;
		}

		_indices.push_back(p1);
		_indices.push_back(p3);
		_indices.push_back(p2);

	}
};

#endif