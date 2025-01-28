#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "boidSetUp.h"
#include "boids.h" 


// Pyramid vertices
GLfloat pyramidVertices[] = {
    // Base (square)
    -0.1f,  0.0f,  0.1f,  1.0f, 0.0f, 0.0f, // Bottom-left
     0.1f,  0.0f,  0.1f,  0.0f, 1.0f, 0.0f, // Bottom-right
     0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 1.0f, // Top-right
    -0.1f,  0.0f, -0.1f,  1.0f, 1.0f, 0.0f, // Top-left

    // Peak (triangle faces)
     0.0f,  0.2f,  0.0f,  1.0f, 1.0f, 1.0f, // Top (shared)
};

GLuint pyramidIndices[] = {
    // Base
    0, 1, 2,
    0, 2, 3,
    // Sides
    0, 1, 4,
    1, 2, 4,
    2, 3, 4,
    3, 0, 4
};

// OpenGL buffers
GLuint pVAO, pVBO, pEBO;


void setupPyramid() {
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1, &pVBO);
    glGenBuffers(1, &pEBO);

    glBindVertexArray(pVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramidIndices), pyramidIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderBoids(std::vector<Boid>& boids, Shader& shaderProgram) {
    for (Boid& boid : boids) {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, boid.position);
        model = glm::rotate(model, glm::radians(boid.angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.05f));

        shaderProgram.Activate();
        shaderProgram.SetMat4("modelMatrix", model);

        shaderProgram.Activate();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        if (boid.context) {
            Core::DrawContext(*boid.context);
        }
    }
}


void setUpBoids(std::vector<Boid>& boids, int numGroups, int numBoidsPerGroup) {
    float spawnRange = 10.0f;

    std::vector<glm::vec3> groupColors = {
    glm::vec3(1.0f, 0.0f, 0.0f), // Red
    glm::vec3(0.0f, 1.0f, 0.0f), // Green
    glm::vec3(0.0f, 0.0f, 1.0f), // Blue
    glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
    glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
    glm::vec3(1.0f, 0.0f, 1.0f), // Magenta
    glm::vec3(1.0f, 0.5f, 0.0f), // Orange
    glm::vec3(0.5f, 0.0f, 0.5f), // Purple
    glm::vec3(0.0f, 0.5f, 0.5f), // Teal
    glm::vec3(0.5f, 0.5f, 0.5f)  // Gray
    };

    for (int group = 0; group < numGroups; ++group) {
        // Assign a unique position offset for each group to separate them
        glm::vec3 groupOffset(
            static_cast<float>(rand() % 200 - 100) / 10.0f,
            static_cast<float>(rand() % 200 - 100) / 10.0f,
            static_cast<float>(rand() % 200 - 100) / 10.0f
        );

        for (int i = 0; i < numBoidsPerGroup; ++i) {
            glm::vec3 startPosition(
                groupOffset.x + static_cast<float>(rand() % 200 - 100) / 10.0f * spawnRange / 10.0f,
                groupOffset.y + static_cast<float>(rand() % 200 - 100) / 10.0f * spawnRange / 10.0f,
                groupOffset.z + static_cast<float>(rand() % 200 - 100) / 10.0f * spawnRange / 10.0f
            );

            glm::vec3 startVelocity(
                static_cast<float>(rand() % 10 - 5) / 10.0f,
                static_cast<float>(rand() % 10 - 5) / 10.0f,
                static_cast<float>(rand() % 10 - 5) / 10.0f
            );

            // Add the boid to the list
            glm::vec3 groupColor = groupColors[group]; 
            boids.emplace_back(startPosition, startVelocity, group, groupColor);
        }
    }
}