#include "..\include\Glyph3D.h"

bool Glyph3D::removeElements(unsigned int b, unsigned int numToRemove)
{
	if (numToRemove == 0) return false;
	if (b < _elements.size())
	{
		if (b + numToRemove <= _elements.size())
		{
			_elements.erase(_elements.begin() + b, _elements.begin() + b + numToRemove);
		}
		else
		{
			std::cout << "the range required to remove is out of range! remove to the end of ContourList." << std::endl;
			_elements.erase(_elements.begin() + b, _elements.end());
		}
		return true;
	}
}

Vec3Array Glyph3D::getNormalArray() const
{
	Vec3Array normal;
	for (const auto &norm : _normals)
		for (const auto &n : norm)
			normal.push_back(n);
	return normal;
}

ElementArray Glyph3D::getIndices() const
{
	ElementArray indices;
	for (const auto &c : _elements)
		for (const auto &i : c)
			indices.push_back(i);

	return indices;
}

void Glyph3D::accept(PrimitiveIndexFunctor& functor) const
{
	const Vec3Array* vertices = &_vertices;

	if (!vertices || vertices->size() == 0) return;

	for (unsigned int i = 0; i < _elements.size(); ++i)
		functor.drawElements(_modeList[i], _elements[i].size(), &(_elements[i].front()));
}

void Glyph3D::output()
{
	std::ofstream fout;
	fout.open("verts_data_3d_I.txt");
	if (!fout)
		std::cout << "verts_data.txt open failed!" << std::endl;

	fout << "Size of vertices: " << _vertices.size() << std::endl;
	fout << "Vertices: " << std::endl;
	for (auto vert : _vertices)
		fout << vert.x << ",  " << vert.y << ",  " << vert.z << std::endl;
	fout << std::endl;

	fout << "Indices:" << std::endl;
	ElementArray indices = getIndices();
	fout << "Size of indices: " << indices.size() << std::endl;
	for (int i = 0; i < indices.size(); ++i)
	{
		if (i % 3 == 0)
			fout << std::endl;
		fout << indices[i] << ", ";
	}
	fout << std::endl << std::endl;

	fout << "Normals:" << std::endl;
	Vec3Array normals = getNormalArray();
	fout << "Size of normals: " << normals.size() << std::endl;
	for (auto norm : normals)
		fout << norm.x << ", " << norm.y << ", " << norm.z << std::endl;
	fout << std::endl;

	fout.close();
}