#include "..\include\Geometry.h"

bool Geometry::removePrimitiveSet(unsigned int b, unsigned int numToRemove)
{
	if (numToRemove == 0) return false;
	if (b < _indices.size())
	{
		if (b + numToRemove <= _indices.size())
		{
			_indices.erase(_indices.begin() + b, _indices.begin() + b + numToRemove);
		}
		else
		{
			std::cout << "要求删除的范围超出数组的最大范围，删除到数组末端！" << std::endl;
			_indices.erase(_indices.begin() + b, _indices.end());
		}
		return true;
	}
}

Vec3Array Geometry::getNormalArray()
{
	Vec3Array normal;
	for (const auto &norm : _normals)
		for (const auto &n : norm)
			normal.push_back(n);
	return normal;
}

ElementArray Geometry::getIndices()
{
	ElementArray indices;
	for (auto iteri = _indices.begin(); iteri != _indices.end(); ++iteri)
		for (auto iterj = (*iteri).begin(); iterj != (*iteri).end(); ++iterj)
			indices.push_back(*iterj);

	return indices;
}

void Geometry::accept(PrimitiveIndexFunctor& functor) const
{
	const Vec3Array* vertices = &_vertices;

	if (!vertices || vertices->size() == 0) return;

	for (unsigned int i = 0; i < _indices.size(); ++i)
	{
		functor.drawElements(_mode_list[i], _indices[i].size(), &(_indices[i].front()));
	}
}

void Geometry::draw(MyShader shader)
{
	updataPrimitives();
	//for (auto &p : _primitive_list)
	p.draw(shader);
}

void Geometry::output()
{
	updataPrimitives();

	std::ofstream fout;
	fout.open("output/verts_data_3d_I.txt");
	if (!fout)
	{
		std::cout << "verts_data.txt open failed!" << std::endl;
	}

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
	{
		fout << norm.x << ", " << norm.y << ", " << norm.z << std::endl;
	}
	fout << std::endl;

	fout.close();
}