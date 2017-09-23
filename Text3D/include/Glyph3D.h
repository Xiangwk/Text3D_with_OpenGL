#pragma once

#include "TriangleIndexFunctor.h"

#include <iostream>
#include <fstream>
#include <vector>

#include <glm\glm.hpp>

using ElementArray = std::vector<unsigned int>;
using Vec3Array = std::vector<glm::vec3>;

class Glyph3D
{
public:
	Glyph3D() = default;
	Glyph3D(const Vec3Array& va, const std::vector<ElementArray>& el) :
		_vertices(va), _elements(el) {}

	//the positions and contours's indices used to do triangulation
	Vec3Array* getVertexPos() { return &_vertices; }
	std::vector<ElementArray>* getElementsArray() { return &_elements; }

	unsigned int getElementsNum() { return _elements.size(); }
	//remove from index b, remove numToRemove's contours
	bool removeElements(unsigned int b, unsigned int numToRemove);

	void addNormalArray(const Vec3Array &normals)
	{
		_normals.push_back(normals);
	}

	//add indices
	void addPrimitiveSet(ElementArray* element) { _elements.push_back(*element); }

	//add primitive mode
	void addMode(GLenum mode) { _modeList.push_back(mode); }
	void clearModeList() { _modeList.clear(); }

	//generate the GL_TRIANGLES indices
	void accept(PrimitiveIndexFunctor& functor) const;

	//the final data that transmited to opengl
	//three face's date(front face, wall face and back face) merge to one mesh
	Vec3Array getNormalArray() const;
	ElementArray getIndices() const;

	//output final data to txt file
	void output();

public:
	Vec3Array _vertices;            //positions
	std::vector<Vec3Array> _normals;           
	std::vector<ElementArray> _elements;       

	//because the gluTess not only generate GL_TRIANGLES mode but also GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP;
	//but we will unify them to GL_TRIANGLES, so we use this vector to store the original data that from gluTess
	//and process them by our PrimitiveIndexFuntor
	std::vector<GLenum> _modeList;
};


struct CollectTriangleIndicesFunctor
{
	CollectTriangleIndicesFunctor() = default;

	using Indices = std::vector<unsigned int>;
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