#include "Render_Utils.h"

#include <algorithm>
#include <glad/glad.h>
#include "freeglut.h"
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>



void Core::RenderContext::initFromAssimpMesh(aiMesh* mesh) {
    // Clear existing buffers
    if (vertexArray != 0) {
        glDeleteVertexArrays(1, &vertexArray);
        vertexArray = 0;
    }
    if (vertexBuffer != 0) {
        glDeleteBuffers(1, &vertexBuffer);
        vertexBuffer = 0;
    }
    if (vertexIndexBuffer != 0) {
        glDeleteBuffers(1, &vertexIndexBuffer);
        vertexIndexBuffer = 0;
    }

    // Check if the mesh has texture coordinates
    bool hasTexCoords = mesh->mTextureCoords[0] != nullptr;
    if (!hasTexCoords) {
        std::cout << "No UV coordinates found in the mesh.\n";
    }

    // Check if the mesh has tangents (required for normal mapping)
    bool hasTangents = mesh->mTangents != nullptr;
    if (!hasTangents) {
        std::cout << "No tangents found in the mesh.\n";
    }

    // Prepare vectors to store vertex data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Iterate through all vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Positions
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normals
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);

        // Texture coordinates (if available)
        if (hasTexCoords) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            vertices.push_back(0.0f); // Default UV
            vertices.push_back(0.0f);
        }

        // Tangents (if available)
        if (hasTangents) {
            vertices.push_back(mesh->mTangents[i].x);
            vertices.push_back(mesh->mTangents[i].y);
            vertices.push_back(mesh->mTangents[i].z);
        }
        else {
            vertices.push_back(1.0f); // Default tangent
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    // Iterate through all faces and store indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Generate and bind the VAO
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Generate and bind the VBO
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Generate and bind the EBO
    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Set up vertex attribute pointers
    // Positions (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);

    // Normals (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));

    // Texture coordinates (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));

    // Tangents (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    // Unbind the VAO
    glBindVertexArray(0);

    // Store the number of indices for rendering
    indexCount = indices.size();

    std::cout << "Mesh loaded successfully with " << mesh->mNumVertices << " vertices and " << indexCount << " indices.\n";
}

void Core::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Core::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Core::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Core::DrawContext(Core::RenderContext& context)
{

    glBindVertexArray(context.vertexArray);
    glDrawElements(GL_TRIANGLES, context.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
