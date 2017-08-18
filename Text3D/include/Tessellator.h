//这是细分器，内部使用glu库中的细分函数完成三角化操作

#ifndef TESSELLATOR_H_
#define TESSELLATOR_H_

#include <iostream>
#include <map>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "Geometry.h"

#define CALLBACK __stdcall
typedef void (CALLBACK * GLU_TESS_CALLBACK)();

class Tessellator
{
public:
	Tessellator();
	~Tessellator();

	//细分多边形
	void retessellatePolygons(Geometry &geom);

	//这个结构用于存储细分后的数据
	struct Prim
	{
		Prim(GLenum mode) :_mode(mode) {}

		typedef std::vector<glm::vec3*> VecList;

		GLenum  _mode;
		VecList _vertices;
	};

	void beginTessellation();

	void beginContour();

	void addVertex(glm::vec3* vertex);

	void endContour();

	void endTessellation();

	typedef std::vector<Prim*> PrimList;

	PrimList& getPrimList() { return _primList; }

	void reset();

private:
	//收集细分好的数据
	void collectTessellation(Geometry &geom, unsigned int originalIndex);

	typedef std::map<glm::vec3*, unsigned int> VertexPtrToIndexMap;
	void addContour(ElementArray* primitive, Vec3Array* vertices);
	void handleNewVertices(Geometry& geom, VertexPtrToIndexMap &vertexPtrToIndexMap);

	void begin(GLenum mode);
	void vertex(glm::vec3* vertex);
	void combine(glm::vec3* vertex, void* vertex_data[4], GLfloat weight[4]);
	void end();
	void error(GLenum errorCode);

	static void CALLBACK beginCallback(GLenum which, void* userData);
	static void CALLBACK vertexCallback(GLvoid *data, void* userData);
	static void CALLBACK combineCallback(GLdouble coords[3], void* vertex_data[4],
		GLfloat weight[4], void** outData,
		void* useData);
	static void CALLBACK endCallback(void* userData);
	static void CALLBACK errorCallback(GLenum errorCode, void* userData);

	struct Vec3d
	{
		double _v[3];
	};

	struct NewVertex
	{

		NewVertex() :
			_vpos(0),
			_f1(0),
			_v1(0),
			_f2(0),
			_v2(0),
			_f3(0),
			_v3(0),
			_f4(0),
			_v4(0) {}

		NewVertex(const NewVertex& nv) :
			_vpos(nv._vpos),
			_f1(nv._f1),
			_v1(nv._v1),
			_f2(nv._f2),
			_v2(nv._v2),
			_f3(nv._f3),
			_v3(nv._v3),
			_f4(nv._f4),
			_v4(nv._v4) {}

		NewVertex(glm::vec3* vx,
			float f1, glm::vec3* v1,
			float f2, glm::vec3* v2,
			float f3, glm::vec3* v3,
			float f4, glm::vec3* v4) :
			_vpos(vx),
			_f1(f1),
			_v1(v1),
			_f2(f2),
			_v2(v2),
			_f3(f3),
			_v3(v3),
			_f4(f4),
			_v4(v4) {}

		glm::vec3  *_vpos;

		float       _f1;
		glm::vec3*  _v1;

		float       _f2;
		glm::vec3*  _v2;

		float       _f3;
		glm::vec3*  _v3;

		float       _f4;
		glm::vec3*  _v4;

	};

	typedef std::vector<NewVertex> NewVertexList;
	typedef std::vector<Vec3d*> Vec3dList;

	GLUtesselator*  _tobj;

	PrimList        _primList;
	Vec3dList       _coordData;
	NewVertexList   _newVertexList;
	GLenum          _errorCode;

	unsigned int _numberVerts;

	std::vector<ElementArray> _Contours;

	unsigned int _index;

	unsigned int _extraPrimitives;
};

void computeGlyphGeometry(Geometry &glyph, float width);

#endif