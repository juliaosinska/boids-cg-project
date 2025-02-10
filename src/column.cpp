#include <glad/glad.h>
#include "column.h"


void initializeColumnOBB(Column& column) {

    glm::vec3 localAxes[3] = {
        glm::vec3(1.0f, 0.0f, 0.0f), // X axis
        glm::vec3(0.0f, 1.0f, 0.0f), // Y axis
        glm::vec3(0.0f, 0.0f, 1.0f)  // Z axis
    };


    glm::vec3 halfExtents = column.size * 0.5f; // we take half the size for our obbs

    glm::vec3 center = column.position;


    column.obb.center = center;
    for (int i = 0; i < 3; i++) {
        column.obb.axes[i] = localAxes[i];
    }
    column.obb.halfExtents = halfExtents;
}

GLfloat columnVertices[COLUMN_VERTEX_COUNT] = {
    // positions          // normals           // texture Coords    // tangents
    // front face
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,      1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,      1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,      1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,      1.0f, 0.0f, 0.0f,

    // back face
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,

    // left face
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,      0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,      0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,      0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,      0.0f, 0.0f, 1.0f,

    // right face
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,      0.0f, 0.0f, -1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,      0.0f, 0.0f, -1.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,      0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,      0.0f, 0.0f, -1.0f,

     // top face
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,     1.0f, 0.0f, 0.0f,

     // bottom face
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,     1.0f, 0.0f, 0.0f
};

GLuint columnIndices[COLUMN_INDEX_COUNT] = {
    // front face
    0, 1, 2, 2, 3, 0,
    // back face
    4, 5, 6, 6, 7, 4,
    // left face
    8, 9, 10, 10, 11, 8,
    // right face
    12, 13, 14, 14, 15, 12,
    // top face
    16, 17, 18, 18, 19, 16,
    // bottom face
    20, 21, 22, 22, 23, 20
};
