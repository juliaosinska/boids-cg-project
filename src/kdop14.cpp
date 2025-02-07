#include "kdop.h"
#include <glfw3.h>

void KDOP14::computeFromMesh(const aiMesh* mesh) {
    if (!mesh) return;

    std::vector<glm::vec3> directions = {
        {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0},
        {0, 0, 1}, {0, 0, -1}, {1, 1, 0}, {-1, -1, 0},
        {1, -1, 0}, {-1, 1, 0}, {1, 0, 1}, {-1, 0, -1},
        {0, 1, 1}, {0, -1, -1}
    };

    std::vector<float> minProj(14, std::numeric_limits<float>::max());
    std::vector<float> maxProj(14, std::numeric_limits<float>::lowest());

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 vertex(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        for (size_t j = 0; j < 14; j++) {
            float projection = glm::dot(vertex, directions[j]);
            minProj[j] = std::min(minProj[j], projection);
            maxProj[j] = std::max(maxProj[j], projection);
        }
    }

    planes.clear();
    for (size_t j = 0; j < 14; j++) {
        planes.push_back({ directions[j], minProj[j] });
        planes.push_back({ directions[j], maxProj[j] });
    }
}

void KDOP14::render() {
    // Iterate over the planes (each plane has a normal and a distance value 'd')
    for (size_t i = 0; i < planes.size(); i += 2) {
        glm::vec3 normal = planes[i].normal;  // Normal of the plane
        float minDist = planes[i].d;          // Min projection value
        float maxDist = planes[i + 1].d;      // Max projection value

        // Calculate points on the plane (simplified for visualization)
        glm::vec3 p1 = minDist * normal * 10.0f;
        glm::vec3 p2 = maxDist * normal * 10.0f;

        // Draw the line representing this plane
        drawLine(p1, p2, glm::vec3(1.0f, 0.0f, 0.0f)); // Red line for each plane
    }
}

void KDOP14::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex3f(start.x, start.y, start.z);
    glVertex3f(end.x, end.y, end.z);
    glEnd();

    std::cout << "im drawing";
}