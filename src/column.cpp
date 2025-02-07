#include "column.h"

void initializeColumnOBB(Column& column) {

    glm::vec3 localAxes[3] = {
        glm::vec3(1.0f, 0.0f, 0.0f), // X axis
        glm::vec3(0.0f, 1.0f, 0.0f), // Y axis
        glm::vec3(0.0f, 0.0f, 1.0f)  // Z axis
    };


    glm::vec3 halfExtents = column.size * 0.5f;

    glm::vec3 center = column.position;


    column.obb.center = center;
    for (int i = 0; i < 3; i++) {
        column.obb.axes[i] = localAxes[i];
    }
    column.obb.halfExtents = halfExtents;

}
