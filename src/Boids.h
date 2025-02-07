#ifndef BOIDS_H
#define BOIDS_H

#include <glm.hpp>
#include <vector>
#include "Render_Utils.h"
#include "obb.h"
#include "Column.h"

// this file just contains the blueprint for the boids class
class Boid {
public:
    // public properties
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    OBB obb; // each boid has their own obb instance stored inside

    glm::vec3 color;

    // boid configuration
    float maxSpeed;
    float maxForce;
    float angle;

    float perceptionRadius;

    int groupID;

    bool hasCollided = false;

    Core::RenderContext* context;

    // constructor
    Boid(glm::vec3 startPosition, glm::vec3 startVelocity, int groupID, glm::vec3 color);

    // update the boid's position and velocity
    void update(const std::vector<Boid>& boids, float deltaTime, const std::vector<Column>& columns);

    // apply a force to the boid
    void applyForce(glm::vec3 force);

    bool isNearBoundary(const glm::vec3& position);
    glm::vec4 getBoundaryNormalAndDistance(glm::vec3& pos);

    glm::vec2 getFishYawAndPitch();
    glm::vec3 getFishVelocity();

    glm::vec3 getBoidPosition(const std::vector<Boid>& boids);
    void handleCollision(Boid& boid1, Boid& boid2);
    void handleCollisionWithColumn(Boid& boid,const Column& column);

    // flocking behaviors
    glm::vec3 separation(const std::vector<Boid>& boids, float desiredSeparation);
    glm::vec3 alignment(const std::vector<Boid>& boids, float neighborDist);
    glm::vec3 cohesion(const std::vector<Boid>& boids, float neighborDist);
};

#endif