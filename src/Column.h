#ifndef COLUMN_H
#define COLUMN_H

#include <glm.hpp>
#include <vector>
#include "obb.h"

struct Column {
    glm::vec3 position; // Column center position
    glm::vec3 size;     // (width, height, depth)
    OBB obb;
};

void initializeColumnOBB(Column& column);

extern std::vector<Column> columns;

#endif  