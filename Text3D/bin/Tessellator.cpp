#include "..\include\Tessellator.h"

Tessellator::Tessellator() :
_numberVerts(0)
{
	_tobj = gluNewTess();
	if (_tobj)
	{
		gluTessCallback(_tobj, GLU_TESS_VERTEX_DATA, (GLU_TESS_CALLBACK)vertexCallback);
		gluTessCallback(_tobj, GLU_TESS_BEGIN_DATA, (GLU_TESS_CALLBACK)beginCallback);
		gluTessCallback(_tobj, GLU_TESS_END_DATA, (GLU_TESS_CALLBACK)endCallback);
		gluTessCallback(_tobj, GLU_TESS_COMBINE_DATA, (GLU_TESS_CALLBACK)combineCallback);
		gluTessCallback(_tobj, GLU_TESS_ERROR_DATA, (GLU_TESS_CALLBACK)errorCallback);
	}
	_errorCode = 0;
	_index = 0;
}

Tessellator::~Tessellator()
{
	reset();
	if (_tobj)
	{
		gluDeleteTess(_tobj);
	}
}

void Tessellator::beginTessellation()
{
	reset();

	if (_tobj)
	{
		gluTessProperty(_tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
		gluTessProperty(_tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE);

		gluTessBeginPolygon(_tobj, this);
	}
}

void Tessellator::beginContour()
{
	if (_tobj)
	{
		gluTessBeginContour(_tobj);
	}
}

void Tessellator::endContour()
{
	if (_tobj)
	{
		gluTessEndContour(_tobj);
	}
}

void Tessellator::endTessellation()
{
	if (_tobj)
	{
		gluTessEndPolygon(_tobj);

		if (_errorCode != 0)
		{
			const GLubyte *estring = gluErrorString((GLenum)_errorCode);
			std::cout << "Tessellation Error: " << estring << std::endl;
		}
	}
}

void Tessellator::reset()
{
	for (Vec3dList::iterator i = _coordData.begin(); i != _coordData.end(); ++i)
	{
		delete (*i);
	}

	for (NewVertexList::iterator j = _newVertexList.begin(); j != _newVertexList.end(); ++j)
	{
		NewVertex& newVertex = (*j);
		delete newVertex._vpos;
		newVertex._vpos = NULL;
	}

	_coordData.clear();
	_newVertexList.clear();
	_primList.clear();
	_errorCode = 0;
}

void Tessellator::retessellatePolygons(Geometry &geom)
{
	// turn the contour list into primitives, a little like Tessellator does but more generally
	Vec3Array* vertices = geom.getVertexArray();

	if (!vertices || vertices->empty() || geom.getPrimitiveSetList().empty()) return;

	_index = 0; // reset the counter for indexed vertices
	_extraPrimitives = 0;
	if (!_numberVerts)
	{
		_numberVerts = geom.getVertexArray()->size();
		// save the contours for complex (winding rule) tessellations
		_Contours = geom.getPrimitiveSetList();
	}

	// remove the existing primitives.
	unsigned int nprimsetoriginal = geom.getNumPrimitiveSets();
	if (nprimsetoriginal) geom.removePrimitiveSet(0, nprimsetoriginal);

	// the main difference from osgUtil::Tessellator for Geometry sets of multiple contours is that the begin/end tessellation
	// occurs around the whole set of contours.
	beginTessellation();
	// process all the contours into the Tessellator
	int noContours = _Contours.size();
	int currentPrimitive = 0;
	for (int primNo = 0; primNo < noContours; ++primNo)
	{
		ElementArray *primitive = &(_Contours[primNo]);

		addContour(primitive, vertices);
	}
	endTessellation();
	collectTessellation(geom, 0);
}

void Tessellator::addContour(ElementArray* primitive, Vec3Array* vertices)
{
	unsigned int idx = 0;

	beginContour();
	ElementArray *drawElements = primitive;
	for (ElementArray::iterator indexItr = drawElements->begin();
		indexItr != drawElements->end();
		++indexItr, idx++)
	{
		addVertex(&((*vertices)[*indexItr]));
	}
	endContour();
}

void Tessellator::addVertex(glm::vec3* vertex)
{
	if (_tobj)
	{
		if (vertex)
		{
			Vec3d* data = new Vec3d;
			_coordData.push_back(data);
			(*data)._v[0] = (*vertex)[0];
			(*data)._v[1] = (*vertex)[1];
			(*data)._v[2] = (*vertex)[2];
			gluTessVertex(_tobj, (data->_v), vertex);
		}
	}
}

void Tessellator::handleNewVertices(Geometry& geom, VertexPtrToIndexMap &vertexPtrToIndexMap)
{
	if (!_newVertexList.empty())
	{
		Vec3Array* vertices = geom.getVertexArray();

		// now add any new vertices that are required.
		for (NewVertexList::iterator itr = _newVertexList.begin();
			itr != _newVertexList.end();
			++itr)
		{
			NewVertex& newVertex = (*itr);
			glm::vec3* vertex = newVertex._vpos;

			// assign vertex.
			vertexPtrToIndexMap[vertex] = vertices->size();
			vertices->push_back(*vertex);
		}
	}
}

void Tessellator::begin(GLenum mode)
{
	_primList.push_back(new Prim(mode));
}

void Tessellator::vertex(glm::vec3* vertex)
{
	if (!_primList.empty())
	{
		Prim* prim = _primList.back();
		prim->_vertices.push_back(vertex);
	}
}

void Tessellator::combine(glm::vec3* vertex, void* vertex_data[4], GLfloat weight[4])
{
	_newVertexList.push_back(NewVertex(vertex,
		weight[0], (glm::vec3*)vertex_data[0],
		weight[1], (glm::vec3*)vertex_data[1],
		weight[2], (glm::vec3*)vertex_data[2],
		weight[3], (glm::vec3*)vertex_data[3]));
}

void Tessellator::end()
{
	// no need to do anything right now...
}

void Tessellator::error(GLenum errorCode)
{
	_errorCode = errorCode;
}

void Tessellator::beginCallback(GLenum which, void* userData)
{
	((Tessellator*)userData)->begin(which);
}

void Tessellator::endCallback(void* userData)
{
	((Tessellator*)userData)->end();
}

void Tessellator::vertexCallback(GLvoid *data, void* userData)
{
	((Tessellator*)userData)->vertex((glm::vec3*)data);
}

void Tessellator::combineCallback(GLdouble coords[3], void* vertex_data[4],
	GLfloat weight[4], void** outData,
	void* userData)
{
	glm::vec3* newData = new glm::vec3(coords[0], coords[1], coords[2]);
	*outData = newData;
	((Tessellator*)userData)->combine(newData, vertex_data, weight);
}

void Tessellator::errorCallback(GLenum errorCode, void* userData)
{
	((Tessellator*)userData)->error(errorCode);
}

void Tessellator::collectTessellation(Geometry &geom, unsigned int originalIndex)
{
	Vec3Array* vertices = geom.getVertexArray();
	VertexPtrToIndexMap vertexPtrToIndexMap;

	// populate the VertexPtrToIndexMap.
	for (unsigned int vi = 0; vi < vertices->size(); ++vi)
	{
		vertexPtrToIndexMap[&((*vertices)[vi])] = vi;
	}

	handleNewVertices(geom, vertexPtrToIndexMap);

	geom.clearModeList();

	for (PrimList::iterator primItr = _primList.begin();
		primItr != _primList.end();
		++primItr, ++_index)
	{
		Prim* prim = *primItr;

		ElementArray elements;
		for (Prim::VecList::iterator vitr = prim->_vertices.begin();
			vitr != prim->_vertices.end();
			++vitr)
		{
			elements.push_back(vertexPtrToIndexMap[*vitr]);
		}
		geom.addMode(prim->_mode);
		// add to the drawn primitive list.
		geom.addPrimitiveSet(&elements);
	}
}

void computeGlyphGeometry(Geometry &glyph, float width)
{
	Vec3Array *vertices = glyph.getVertexArray();
	Vec3Array origin_vertices = *vertices;
	Vec3Array normals;
	std::vector<ElementArray> origin_indices = glyph.getPrimitiveSetList();
	//std::cout << origin_indices.size() << std::endl;

	//将原始字形进行三角化
	Tessellator ts;
	ts.retessellatePolygons(glyph);

	//将处理过的字形的索引全部转化为三角索引
	//注：GLU库三角化可能生成三种类型的图元，GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
	TriangleIndexFunctor<CollectTriangleIndicesFunctor> ctif;
	glyph.accept(ctif);
	CollectTriangleIndicesFunctor::Indices& indices = ctif._indices;

	//将原始图元列表清空，这里要重新构建三角图元列表
	//其中每一个封闭曲面为一个绘制图元，整个字形几何体共用顶点数组
	glyph.getPrimitiveSetList().clear();
	glyph.clearModeList();

	if (indices.empty()) std::cout << "The new indices create failed!" << std::endl;

	//构建3D文字前向面索引
	ElementArray front_face;
	for (unsigned int i = 0; i < indices.size(); ++i)
	{
		front_face.push_back(indices[i]);
		normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	/*for (unsigned int i = 0; i < origin_vertices.size(); ++i)
	normals.push_back(glm::vec3(0.0f, 0.0f, 1.0f));*/

	glyph.addPrimitiveSet(&front_face);
	glyph.addNormalArray(normals);
	glyph.addMode(GL_TRIANGLES);

	//构建3D文字后向面索引
	const unsigned int NULL_VALUE = UINT_MAX;
	ElementArray back_indices;
	back_indices.resize(vertices->size(), NULL_VALUE);
	glm::vec3 forward(0, 0, -width);
	normals.clear();

	ElementArray back_face;
	for (unsigned int i = 0; i < indices.size() - 2;)
	{
		unsigned int p1 = indices[i++];
		unsigned int p2 = indices[i++];
		unsigned int p3 = indices[i++];
		if (back_indices[p1] == NULL_VALUE)
		{
			back_indices[p1] = vertices->size();
			vertices->push_back((*vertices)[p1] + forward);
		}

		if (back_indices[p2] == NULL_VALUE)
		{
			back_indices[p2] = vertices->size();
			vertices->push_back((*vertices)[p2] + forward);
		}

		if (back_indices[p3] == NULL_VALUE)
		{

			back_indices[p3] = vertices->size();
			vertices->push_back((*vertices)[p3] + forward);
		}

		back_face.push_back(back_indices[p1]);
		back_face.push_back(back_indices[p3]);
		back_face.push_back(back_indices[p2]);

		normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
		normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	}
	/*for (unsigned int i = 0; i < origin_vertices.size(); ++i)
	normals.push_back(glm::vec3(0.0f, 0.0f, -1.0f));*/
	glyph.addPrimitiveSet(&back_face);
	glyph.addNormalArray(normals);
	glyph.addMode(GL_TRIANGLES);

	//构建3D文字侧表面索引
	unsigned int orig_size = origin_vertices.size();
	ElementArray frontedge_indices, backedge_indices;
	frontedge_indices.resize(orig_size, NULL_VALUE);
	backedge_indices.resize(orig_size, NULL_VALUE);

	for (ElementArray elements : origin_indices)
	{
		normals.clear();

		ElementArray edging;
		if (!elements.empty())
		{
			for (unsigned int i = 0; i < elements.size(); ++i)
			{
				unsigned int ei = elements[i];
				if (frontedge_indices[ei] == NULL_VALUE)
				{
					frontedge_indices[ei] = vertices->size();
					vertices->push_back(origin_vertices[ei]);
				}
				if (backedge_indices[ei] == NULL_VALUE)
				{
					backedge_indices[ei] = vertices->size();
					vertices->push_back(origin_vertices[ei] + forward);
				}

				edging.push_back(backedge_indices[ei]);
				edging.push_back(frontedge_indices[ei]);
			}
		}

		std::vector<unsigned int> wall_face;
		GLuint *iptr = &edging.front();
		for (unsigned int i = 3; i < edging.size(); i += 2, iptr += 2)
		{
			glm::vec3 edge1((*vertices)[*(iptr + 1)] - (*vertices)[*iptr]);
			glm::vec3 edge2((*vertices)[*(iptr + 2)] - (*vertices)[*iptr]);
			glm::vec3 temp_normal = glm::cross(edge1, edge2);
			wall_face.push_back(*(iptr));
			wall_face.push_back(*(iptr + 1));
			wall_face.push_back(*(iptr + 2));

			temp_normal = glm::normalize(temp_normal);

			normals.push_back(temp_normal);
			normals.push_back(temp_normal);
			normals.push_back(temp_normal);

			wall_face.push_back(*(iptr + 1));
			wall_face.push_back(*(iptr + 3));
			wall_face.push_back(*(iptr + 2));

			normals.push_back(temp_normal);
			normals.push_back(temp_normal);
			normals.push_back(temp_normal);
		}
		glyph.addPrimitiveSet(&wall_face);
		glyph.addNormalArray(normals);
		glyph.addMode(GL_TRIANGLES);
	}
}