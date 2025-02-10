#ifndef OBB_H
#define OBB_H

#include <glm.hpp>
#include <vector>

// OBB struct
struct OBB {
    glm::vec3 center;      
    glm::vec3 axes[3];     
    glm::vec3 halfExtents; 
};

// function to update the OBB based on the boid's transformation
void updateOBB(const glm::mat4& transform, const glm::vec3& localCenter, const glm::vec3 localAxes[3], OBB& obb);

// function to project an OBB onto an axis
float projectOBB(const OBB& obb, const glm::vec3& axis);

// function to check if two OBBs overlap on a specific axis
bool overlapOnAxis(const OBB& obb1, const OBB& obb2, const glm::vec3& axis);

// function to check for collision between two OBBs using SAT
bool checkOBBCollision(const OBB& obb1, const OBB& obb2);

std::vector<glm::vec3> getOBBVertices(const OBB& obb);

void drawLine(const glm::vec3& start, const glm::vec3& end);

void renderOBB(const OBB& obb);



#endif 
