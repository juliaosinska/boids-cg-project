#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "ShaderClass.h"
#include "boidSetUp.h"
#include "boids.h" 
#include "Render_Utils.h"
#include "obb.h"


// pyramid vertices (initial boid structure -> changed to fish model)
GLfloat pyramidVertices[] = {
    // base (square)
    -0.1f,  0.0f,  0.1f,  1.0f, 0.0f, 0.0f, // bottom-left
     0.1f,  0.0f,  0.1f,  0.0f, 1.0f, 0.0f, // bottom-right
     0.1f,  0.0f, -0.1f,  0.0f, 0.0f, 1.0f, // top-right
    -0.1f,  0.0f, -0.1f,  1.0f, 1.0f, 0.0f, // top-left

    // peak (triangle faces)
     0.0f,  0.2f,  0.0f,  1.0f, 1.0f, 1.0f, // top (shared)
};

GLuint pyramidIndices[] = {
    // base
    0, 1, 2,
    0, 2, 3,
    // sides
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
        glm::vec3 forward = boid.getFishVelocity();
        
        if (glm::length(forward) < 1e-6f) {
            forward = glm::vec3(1.0f, 0.0f, 0.0f); // default all fish to face right if no movement
        }
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); // world up
        glm::vec3 right = glm::normalize(glm::cross(up, forward)); // perpendicular right
        glm::vec3 adjustedUp = glm::cross(forward, right);

        // create a rotation matrix from these vectors
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation[0] = glm::vec4(right, 0.0f); 
        rotation[1] = glm::vec4(adjustedUp, 0.0f); 
        rotation[2] = glm::vec4(forward, 0.0f);
        
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, boid.position);
        model *= rotation; // makes our fish more bendy and natural
        model = glm::scale(model, glm::vec3(0.1f));

        // update the obb of each boid - send the current roatation to get the obb faced correctly
        updateOBB(model, glm::vec3(0.0f), boid.obb.axes, boid.obb);
        
        shaderProgram.Activate();
        shaderProgram.SetMat4("modelMatrix", model);
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
        // assign a unique position offset for each group to separate them
        glm::vec3 groupOffset(
            static_cast<float>(rand() % 220 - 100) / 10.0f,
            static_cast<float>(rand() % 200 - 100) / 10.0f,
            static_cast<float>(rand() % 240 - 100) / 10.0f
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

            // add the boid to the list
            glm::vec3 groupColor = groupColors[group]; 
            boids.emplace_back(startPosition, startVelocity, group, groupColor);
        }
    }
}