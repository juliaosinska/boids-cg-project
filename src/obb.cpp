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
    // Update the OBB center - we take the local center of the fish, so our extends will be correct
    obb.center = glm::vec3(transform * glm::vec4(localCenter, 1.0f));
    
    //std::cout << obb.center.x; std::cout << std::endl; std::cout << obb.center.y;

    // Update the OBB axes (rotate the local axes by the boid's rotation)
    for (int i = 0; i < 3; i++) {
        obb.axes[i] = glm::normalize(glm::vec3(transform * glm::vec4(localAxes[i], 0.0f)));
    }
}

// Function to project an OBB onto an axis - this will allow us to use SAT to check for collisions
float projectOBB(const OBB& obb, const glm::vec3& axis) {
    return glm::dot(obb.halfExtents, glm::abs(glm::vec3(
        glm::dot(axis, obb.axes[0]),
        glm::dot(axis, obb.axes[1]),
        glm::dot(axis, obb.axes[2])
    )));
}

// Function to check if two OBBs overlap on a specific axis
bool overlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis) {
    float projection1 = projectOBB(obb1, axis);
    float projection2 = projectOBB(obb2, axis);
    float distance = glm::abs(glm::dot(obb1.center - obb2.center, axis));
    return distance < (projection1 + projection2);
}

// Function to check for collision between two OBBs using SAT
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

    // If no separating axis is found, the OBBs are colliding !!!!
    return true;
}

std::vector<glm::vec3> getOBBVertices(const OBB& obb) {
    std::vector<glm::vec3> vertices(8);

    // The 8 vertices of the OBB
    for (int i = 0; i < 8; ++i) {
        glm::vec3 sign(
            (i & 1) ? 1.0f : -1.0f,   // x-axis sign
            (i & 2) ? 1.0f : -1.0f,   // y-axis sign
            (i & 4) ? 1.0f : -1.0f    // z-axis sign
        );

        // Each vertex is the center plus or minus half-extents along each axis
        vertices[i] = obb.center + obb.axes[0] * sign.x * obb.halfExtents.x
            + obb.axes[1] * sign.y * obb.halfExtents.y
            + obb.axes[2] * sign.z * obb.halfExtents.z;
    }

    return vertices;
}

//void renderOBB(const OBB& obb) {
    //std::vector<glm::vec3> vertices = getOBBVertices(obb);

    //// Connect the vertices to draw the edges
    //// Bottom square
    //drawLine(vertices[0], vertices[1]);
    //drawLine(vertices[1], vertices[3]);
    //drawLine(vertices[3], vertices[2]);
    //drawLine(vertices[2], vertices[0]);

    //// Top square
    //drawLine(vertices[4], vertices[5]);
    //drawLine(vertices[5], vertices[7]);
    //drawLine(vertices[7], vertices[6]);
    //drawLine(vertices[6], vertices[4]);

    //// Connect top and bottom
    //drawLine(vertices[0], vertices[4]);
    //drawLine(vertices[1], vertices[5]);
    //drawLine(vertices[2], vertices[6]);
    //drawLine(vertices[3], vertices[7]);
//}

//void drawLine(const glm::vec3& start, const glm::vec3& end) {
//    // OpenGL code to draw a line between start and end (for example)
//    glBegin(GL_LINES);
//    glVertex3f(start.x, start.y, start.z);
//    glVertex3f(end.x, end.y, end.z);
//    glEnd();
//}