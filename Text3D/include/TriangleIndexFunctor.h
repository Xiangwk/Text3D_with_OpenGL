#ifndef TRIANGLEINDEXFUNCTOR_H_
#define TRIANGLEINDEXFUNCTOR_H_

#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

class PrimitiveIndexFunctor
{
public:

	virtual ~PrimitiveIndexFunctor() {}

	virtual void setVertexArray(unsigned int count, const glm::vec2* vertices) = 0;
	virtual void setVertexArray(unsigned int count, const glm::vec3* vertices) = 0;
	virtual void setVertexArray(unsigned int count, const glm::vec4* vertices) = 0;

	virtual void drawArrays(GLenum mode, GLint first, GLsizei count) = 0;
	virtual void drawElements(GLenum mode, GLsizei count, const GLubyte* indices) = 0;
	virtual void drawElements(GLenum mode, GLsizei count, const GLushort* indices) = 0;
	virtual void drawElements(GLenum mode, GLsizei count, const GLuint* indices) = 0;

	virtual void begin(GLenum mode) = 0;
	virtual void vertex(unsigned int pos) = 0;
	virtual void end() = 0;

	void useVertexCacheAsVertexArray()
	{
		setVertexArray(_vertexCache.size(), &_vertexCache.front());
	}

	std::vector<glm::vec3>   _vertexCache;
	bool                _treatVertexDataAsTemporary;
};

template<class T>
class TriangleIndexFunctor : public PrimitiveIndexFunctor, public T
{
public:


	virtual void setVertexArray(unsigned int, const glm::vec2*)
	{
	}

	virtual void setVertexArray(unsigned int, const glm::vec3*)
	{
	}

	virtual void setVertexArray(unsigned int, const glm::vec4*)
	{
	}

	virtual void begin(GLenum mode)
	{
		_modeCache = mode;
		_indexCache.clear();
	}

	virtual void vertex(unsigned int vert)
	{
		_indexCache.push_back(vert);
	}

	virtual void end()
	{
		if (!_indexCache.empty())
		{
			drawElements(_modeCache, _indexCache.size(), &_indexCache.front());
		}
	}

	virtual void drawArrays(GLenum mode, GLint first, GLsizei count)
	{
		switch (mode)
		{
		case(GL_TRIANGLES) :
		{
			unsigned int pos = first;
			for (GLsizei i = 2; i<count; i += 3, pos += 3)
			{
				this->operator()(pos, pos + 1, pos + 2);
			}
			break;
		}
		case(GL_TRIANGLE_STRIP) :
		{
			unsigned int pos = first;
			for (GLsizei i = 2; i<count; ++i, ++pos)
			{
				if ((i % 2)) this->operator()(pos, pos + 2, pos + 1);
				else       this->operator()(pos, pos + 1, pos + 2);
			}
			break;
		}
		case(GL_QUADS) :
		{
			unsigned int pos = first;
			for (GLsizei i = 3; i<count; i += 4, pos += 4)
			{
				this->operator()(pos, pos + 1, pos + 2);
				this->operator()(pos, pos + 2, pos + 3);
			}
			break;
		}
		case(GL_QUAD_STRIP) :
		{
			unsigned int pos = first;
			for (GLsizei i = 3; i<count; i += 2, pos += 2)
			{
				this->operator()(pos, pos + 1, pos + 2);
				this->operator()(pos + 1, pos + 3, pos + 2);
			}
			break;
		}
		case(GL_POLYGON) : // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN) :
		{
			unsigned int pos = first + 1;
			for (GLsizei i = 2; i<count; ++i, ++pos)
			{
				this->operator()(first, pos, pos + 1);
			}
			break;
		}
		case(GL_POINTS) :
		case(GL_LINES) :
		case(GL_LINE_STRIP) :
		case(GL_LINE_LOOP) :
		default:
			// can't be converted into to triangles.
			break;
		}
	}

	virtual void drawElements(GLenum mode, GLsizei count, const GLubyte* indices)
	{
		if (indices == 0 || count == 0) return;

		typedef GLubyte Index;
		typedef const Index* IndexPointer;

		switch (mode)
		{
		case(GL_TRIANGLES) :
		{
			IndexPointer ilast = &indices[count];
			for (IndexPointer iptr = indices; iptr<ilast; iptr += 3)
				this->operator()(*iptr, *(iptr + 1), *(iptr + 2));
			break;
		}
		case(GL_TRIANGLE_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				if ((i % 2)) this->operator()(*(iptr), *(iptr + 2), *(iptr + 1));
				else       this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
			}
			break;
		}
		case(GL_QUADS) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 4, iptr += 4)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr), *(iptr + 2), *(iptr + 3));
			}
			break;
		}
		case(GL_QUAD_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 2, iptr += 2)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr + 1), *(iptr + 3), *(iptr + 2));
			}
			break;
		}
		case(GL_POLYGON) : // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN) :
		{
			IndexPointer iptr = indices;
			Index first = *iptr;
			++iptr;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				this->operator()(first, *(iptr), *(iptr + 1));
			}
			break;
		}
		case(GL_POINTS) :
		case(GL_LINES) :
		case(GL_LINE_STRIP) :
		case(GL_LINE_LOOP) :
		default:
			// can't be converted into to triangles.
			break;
		}
	}

	virtual void drawElements(GLenum mode, GLsizei count, const GLushort* indices)
	{
		if (indices == 0 || count == 0) return;

		typedef GLushort Index;
		typedef const Index* IndexPointer;

		switch (mode)
		{
		case(GL_TRIANGLES) :
		{
			IndexPointer ilast = &indices[count];
			for (IndexPointer iptr = indices; iptr<ilast; iptr += 3)
				this->operator()(*iptr, *(iptr + 1), *(iptr + 2));
			break;
		}
		case(GL_TRIANGLE_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				if ((i % 2)) this->operator()(*(iptr), *(iptr + 2), *(iptr + 1));
				else       this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
			}
			break;
		}
		case(GL_QUADS) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 4, iptr += 4)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr), *(iptr + 2), *(iptr + 3));
			}
			break;
		}
		case(GL_QUAD_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 2, iptr += 2)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr + 1), *(iptr + 3), *(iptr + 2));
			}
			break;
		}
		case(GL_POLYGON) : // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN) :
		{
			IndexPointer iptr = indices;
			Index first = *iptr;
			++iptr;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				this->operator()(first, *(iptr), *(iptr + 1));
			}
			break;
		}
		case(GL_POINTS) :
		case(GL_LINES) :
		case(GL_LINE_STRIP) :
		case(GL_LINE_LOOP) :
		default:
			// can't be converted into to triangles.
			break;
		}
	}

	virtual void drawElements(GLenum mode, GLsizei count, const GLuint* indices)
	{
		if (indices == 0 || count == 0) return;

		typedef GLuint Index;
		typedef const Index* IndexPointer;

		switch (mode)
		{
		case(GL_TRIANGLES) :
		{
			IndexPointer ilast = &indices[count];
			for (IndexPointer iptr = indices; iptr<ilast; iptr += 3)
				this->operator()(*iptr, *(iptr + 1), *(iptr + 2));
			break;
		}
		case(GL_TRIANGLE_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				if ((i % 2)) this->operator()(*(iptr), *(iptr + 2), *(iptr + 1));
				else       this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
			}
			break;
		}
		case(GL_QUADS) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 4, iptr += 4)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr), *(iptr + 2), *(iptr + 3));
			}
			break;
		}
		case(GL_QUAD_STRIP) :
		{
			IndexPointer iptr = indices;
			for (GLsizei i = 3; i<count; i += 2, iptr += 2)
			{
				this->operator()(*(iptr), *(iptr + 1), *(iptr + 2));
				this->operator()(*(iptr + 1), *(iptr + 3), *(iptr + 2));
			}
			break;
		}
		case(GL_POLYGON) : // treat polygons as GL_TRIANGLE_FAN
		case(GL_TRIANGLE_FAN) :
		{
			IndexPointer iptr = indices;
			Index first = *iptr;
			++iptr;
			for (GLsizei i = 2; i<count; ++i, ++iptr)
			{
				this->operator()(first, *(iptr), *(iptr + 1));
			}
			break;
		}
		case(GL_POINTS) :
		case(GL_LINES) :
		case(GL_LINE_STRIP) :
		case(GL_LINE_LOOP) :
		default:
			// can't be converted into to triangles.
			break;
		}
	}

	GLenum               _modeCache;
	std::vector<GLuint>  _indexCache;
};

#endif