#ifndef BOIDS_H
#define BOIDS_H

#include <glm.hpp>
#include <vector>
#include "Render_Utils.h"

// this file just contains the blueprint for the boids class
class Boid {
public:
    // public properties
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    glm::vec3 color;

    // boid configuration
    float maxSpeed;
    float maxForce;
    float angle;

    float perceptionRadius;

    int groupID;

    Core::RenderContext* context;

    // constructor
    Boid(glm::vec3 startPosition, glm::vec3 startVelocity, int groupID, glm::vec3 color);

    // update the boid's position and velocity
    void update(const std::vector<Boid>& boids);

    // apply a force to the boid
    void applyForce(glm::vec3 force);

    bool isNearBoundary(const glm::vec3& position);

    // flocking behaviors
    glm::vec3 separation(const std::vector<Boid>& boids, float desiredSeparation);
    glm::vec3 alignment(const std::vector<Boid>& boids, float neighborDist);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float neighborDist);
};

#endif