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
    }

    void generateTerrain() {
        PerlinNoise perlin;

        for (int z = 0; z < height; z++) {
            for (int x = 0; x < width; x++) {
                float y = perlin.noise(x * 0.1f, 0.0f, z * 0.1f) * scale;

                // Wspó³rzêdne wierzcho³ków (x, y, z)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normale (proste na razie w górê)
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
                vertices.push_back(0.0f);

                // Wspó³rzêdne UV dla tekstury
                vertices.push_back((float)x / (float)width);
                vertices.push_back((float)z / (float)height);
            }
        }


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
};

#endif
