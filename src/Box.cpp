// box.cpp
#include "Box.h"

Box::Box() {
    boxVertices = {
        // Coordinates        // Colors           // Normals
        -0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Front face
         0.1f, -0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f, // Back face
         0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   1.0f, -1.0f, // Back face
        -0.1f,  0.1f,  0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Front face

        -0.1f, -0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f,  // Top face
         0.1f, -0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, -1.0f, // Bottom face
         0.1f,  0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, -1.0f, // Bottom face
        -0.1f,  0.1f, -0.1f,  0.8f, 0.52f, 0.25f,   0.0f, 1.0f   // Top face
    };

   boxIndices = {
        // Front face
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    // Back face
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    // Left face
    0, 4,
    1, 5,
    2, 6,
    3, 7,

    // Right face
    0, 5,
    1, 4,
    2, 7,
    3, 6,

    // Top face
    3, 2,
    7, 6,

    // Bottom face
    0, 1,
    4, 5
    };
};