#ifndef BOX_H
#define BOX_H
#define GLM_ENABLE_EXPERIMENTAL
#include <glad.h>
#include <vector>


struct Box {
    std::vector<GLfloat> boxVertices;
    std::vector<GLuint> boxIndices;

    Box();
};

#endif 