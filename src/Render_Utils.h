#pragma once
#include <glad/glad.h>
#include "glm.hpp"
#include "objload.h"
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Core
{
	struct RenderContext
    {
		GLuint vertexArray;
		GLuint vertexBuffer;
		GLuint vertexIndexBuffer;
		int size = 0;
		unsigned int indexCount;

        void initFromOBJ(obj::Model& model);

		void initFromAssimpMesh(aiMesh* mesh);
	};

	void DrawVertexArray(const float * vertexArray, int numVertices, int elementSize);

	void DrawVertexArrayIndexed(const float * vertexArray, const int * indexArray, int numIndexes, int elementSize);

	struct VertexAttribute
	{
		const void * Pointer;
		int Size;
	};

	struct VertexData
	{
		static const int MAX_ATTRIBS = 8;
		VertexAttribute Attribs[MAX_ATTRIBS];
		int NumActiveAttribs;
		int NumVertices;
	};

	void DrawVertexArray(const VertexData & data);

	void DrawContext(RenderContext& context);
}