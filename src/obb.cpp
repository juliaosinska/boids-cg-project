#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <iostream>



// OBB struct
struct OBB {
    glm::vec3 center;      // center of the box in world space - we will take our boid position as this
    glm::vec3 axes[3];     // local x, y, z axes of the box (normalized)
    glm::vec3 halfExtents; // half the size of the box along each axis - creates a box around our boid
};

// function to update the OBB based on the boid's transformation
void updateOBB(const glm::mat4& transform, const glm::vec3& localCenter, const glm::vec3 localAxes[3], OBB& obb) {
    // update the OBB center - we take the local center of the fish, so our extends will be correct
    obb.center = glm::vec3(transform * glm::vec4(localCenter, 1.0f));

    // update the OBB axes (rotate the local axes by the boid's rotation)
    for (int i = 0; i < 3; i++) {
        obb.axes[i] = glm::normalize(glm::vec3(transform * glm::vec4(localAxes[i], 0.0f)));
    }
}

// function to project an OBB onto an axis - this will allow us to use SAT to check for collisions
float projectOBB(const OBB& obb, const glm::vec3& axis) {
    return glm::dot(obb.halfExtents, glm::abs(glm::vec3(
        glm::dot(axis, obb.axes[0]),
        glm::dot(axis, obb.axes[1]),
        glm::dot(axis, obb.axes[2])
    )));
}

// function to check if two OBBs overlap on a specific axis
bool overlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis) {
    float projection1 = projectOBB(obb1, axis);
    float projection2 = projectOBB(obb2, axis);
    float distance = glm::abs(glm::dot(obb1.center - obb2.center, axis));
    return distance < (projection1 + projection2);
}

// function to check for collision between two OBBs using SAT
bool checkOBBCollision(const OBB& obb1, const OBB& obb2) {
    // rest all 15 potential separating axes
    for (int i = 0; i < 3; i++) {
        
        if (!overlapOnAxis(obb1, obb2, obb1.axes[i])) return false;
        if (!overlapOnAxis(obb1, obb2, obb2.axes[i])) return false;

    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            glm::vec3 axis = glm::cross(obb1.axes[i], obb2.axes[j]);
            if (glm::length(axis) < 1e-6f) continue; // we skip parallel axes
            axis = glm::normalize(axis);
            if (!overlapOnAxis(obb1, obb2, axis)) return false;
        }
    }

    // if no separating axis is found, the OBBs are colliding !!!!
    return true;
}

std::vector<glm::vec3> getOBBVertices(const OBB& obb) {
    std::vector<glm::vec3> vertices(8);

    // the 8 vertices of the OBB
    for (int i = 0; i < 8; ++i) {
        glm::vec3 sign(
            (i & 1) ? 1.0f : -1.0f,   // x-axis sign
            (i & 2) ? 1.0f : -1.0f,   // y-axis sign
            (i & 4) ? 1.0f : -1.0f    // z-axis sign
        );

        // each vertex is the center plus or minus half-extents along each axis
        vertices[i] = obb.center + obb.axes[0] * sign.x * obb.halfExtents.x
            + obb.axes[1] * sign.y * obb.halfExtents.y
            + obb.axes[2] * sign.z * obb.halfExtents.z;
    }

    return vertices;
}