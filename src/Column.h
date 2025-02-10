#ifndef COLUMN_H
#define COLUMN_H

#include <glm.hpp>
#include <vector>
#include "obb.h"

struct Column {
    glm::vec3 position; // column center position
    glm::vec3 size;     // (width, height, depth)
    OBB obb;
};

void initializeColumnOBB(Column& column);

extern std::vector<Column> columns;

constexpr size_t COLUMN_VERTEX_COUNT = 24 * 11;
constexpr size_t COLUMN_INDEX_COUNT = 36;

extern GLfloat columnVertices[COLUMN_VERTEX_COUNT];
extern GLuint columnIndices[COLUMN_INDEX_COUNT];

#endif  