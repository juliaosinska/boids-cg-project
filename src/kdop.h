#ifndef KDOP_H
#define KDOP_H

#include <vector>
#include <iostream>
#include <limits>
#include <glm.hpp>
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>

class KDOP14 {
public:
    struct Plane {
        glm::vec3 normal; // normal of the plane
        float d;          // Plane equation: N . P = d
    };

    std::vector<Plane> planes;

    void computeFromMesh(const aiMesh* mesh);
    void render();
    void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color);
};

template <typename T>
void printVector(const std::vector<T>& v);

#endif // KDOP_H
