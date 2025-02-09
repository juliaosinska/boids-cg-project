#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include<glm.hpp>
#include "PerlinNoise.h"

class Terrain {
public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int width, height;
    float scale;

    Terrain(int w, int h, float s) : width(w), height(h), scale(s) {
        generateTerrain();
        calculateNormals();
    }

private:
    void generateTerrain() {
        PerlinNoise perlin;

        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                float y = perlin.noise(x * 0.1f, 0.0f, z * 0.1f) * scale;

                // Vertex position (x, y, z)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Placeholder normals (will be updated)
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);

                // UV coordinates
                vertices.push_back((float)x / (float)width);
                vertices.push_back((float)z / (float)height);
            }
        }

        // Generating indices for triangle strips
        for (int z = 0; z < height - 1; z++) {
            for (int x = 0; x < width - 1; x++) {
                int start = z * width + x;
                indices.push_back(start);
                indices.push_back(start + width);
                indices.push_back(start + 1);

                indices.push_back(start + 1);
                indices.push_back(start + width);
                indices.push_back(start + width + 1);
            }
        }
    }

    void calculateNormals() {
        // Temporary storage for normals
        std::vector<glm::vec3> tempNormals(vertices.size() / 8, glm::vec3(0.0f, 0.0f, 0.0f));

        // Compute normals for each triangle
        for (size_t i = 0; i < indices.size(); i += 3) {
            int i0 = indices[i];
            int i1 = indices[i + 1];
            int i2 = indices[i + 2];

            glm::vec3 v0(vertices[i0 * 8], vertices[i0 * 8 + 1], vertices[i0 * 8 + 2]);
            glm::vec3 v1(vertices[i1 * 8], vertices[i1 * 8 + 1], vertices[i1 * 8 + 2]);
            glm::vec3 v2(vertices[i2 * 8], vertices[i2 * 8 + 1], vertices[i2 * 8 + 2]);

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));

            tempNormals[i0] += normal;
            tempNormals[i1] += normal;
            tempNormals[i2] += normal;
        }

        // Normalize and assign normals to vertices
        for (size_t i = 0; i < tempNormals.size(); i++) {
            tempNormals[i] = glm::normalize(tempNormals[i]);

            vertices[i * 8 + 3] = tempNormals[i].x;
            vertices[i * 8 + 4] = tempNormals[i].y;
            vertices[i * 8 + 5] = tempNormals[i].z;
        }
        for (size_t i = 0; i < tempNormals.size(); i++) {
            std::cout << "Vertex " << i << " Normal: ("
                << tempNormals[i].x << ", "
                << tempNormals[i].y << ", "
                << tempNormals[i].z << ")\n";
        }

    }
};

#endif
