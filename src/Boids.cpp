#include "Boids.h"


// tutaj mozna cos zdynamizowac, idk dac uzytkownikowi moc zmieniania maxSpeed boidow?
Boid::Boid(glm::vec3 startPosition, glm::vec3 startVelocity, int groupID, glm::vec3 color)
    : position(startPosition),
    velocity(startVelocity),
    acceleration(glm::vec3(0.0f)),
    maxSpeed(0.02f),
    maxForce(0.01f),
    angle(0.0f),
    perceptionRadius(2.5f),
    groupID(groupID),
    color(color)
{}

void Boid::update(const std::vector<Boid>& boids) {
    // define cube boundaries - if we want a bigger aquarium, gotta change it here
    const float boundaryThreshold = 1.0f;
    const float cubeMin = -10.0f;
    const float cubeMax = 10.0f;

    // Define weights for the behaviors
    float alignWeight = 0.4f; //zmniejszone sprawia ze ryby w malych stadach tak dziko nie wibruj¹
    float cohesionWeight = 0.02f;
    float separationWeight = 0.3f;

    float wallAvoidanceWeight = 0.5f; 

    glm::vec3 alignForce = alignment(boids, perceptionRadius) * alignWeight;
    glm::vec3 cohesionForce = cohesion(boids, perceptionRadius) * cohesionWeight;
    glm::vec3 separationForce = separation(boids, 1.0f) * separationWeight; // the allowed distance for boids to be in

    glm::vec4 wallNormalAndDistance = getBoundaryNormalAndDistance(position);
    glm::vec3 wallNormal = glm::vec3(wallNormalAndDistance);
    float distToWall = wallNormalAndDistance.w;
    
    // this will cause our fish to turn gracefully
    if (distToWall < 5.0f) {
        float avoidStrength = glm::clamp(1.0f - (distToWall / 5.0f), 0.0f, 1.0f); // Stronger closer to the wall
        glm::vec3 desiredDirection = glm::normalize(velocity) + wallNormal * avoidStrength;
        desiredDirection = glm::normalize(desiredDirection);

        glm::vec3 steering = desiredDirection * maxSpeed - velocity;
        if (glm::length(steering) > maxForce) {
            steering = glm::normalize(steering) * maxForce;
        }

        applyForce(steering * wallAvoidanceWeight);
    }

    applyForce(alignForce);
    applyForce(cohesionForce);
    applyForce(separationForce);
    
    // make the velocity a bit smooter - the fish wont snap so hard anymore :(

    if (glm::length(acceleration) > maxForce) {
        acceleration = glm::normalize(acceleration) * maxForce;
    }

    velocity = glm::mix(velocity, velocity + acceleration, 0.2f); 
   
    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    
    position += velocity;
    
    //if a fish manages to go through our wall avoidence, this will stop them from popping out of the box
    if (position.x < cubeMin) position.x = cubeMin + boundaryThreshold;
    if (position.x > cubeMax) position.x = cubeMax - boundaryThreshold;
    if (position.y < cubeMin) position.y = cubeMin + boundaryThreshold;
    if (position.y > cubeMax) position.y = cubeMax - boundaryThreshold;
    if (position.z < cubeMin) position.z = cubeMin + boundaryThreshold;
    if (position.z > cubeMax) position.z = cubeMax - boundaryThreshold;
    

    // reset acceleration for the next frame
    acceleration = glm::vec3(0.0f);

    // update the boid's angle based on its velocity
    angle = glm::degrees(atan2(velocity.z, velocity.x));
}

glm::vec2 Boid::getFishYawAndPitch() {
    float yaw = glm::degrees(atan2(velocity.z, velocity.x));

    float horizontalSpeed = glm::length(glm::vec2(velocity.x, velocity.z));
    if (horizontalSpeed < 1e-6f) horizontalSpeed = 1e-6f; // prevent division by zero

    float pitch = glm::degrees(atan2(velocity.y, horizontalSpeed));

    glm::vec3 forward = glm::normalize(velocity);

    return glm::vec2(yaw, pitch);
}

glm::vec3 Boid::getFishVelocity() {
    glm::vec3 forward = glm::normalize(velocity);
    return forward;
}

glm::vec4 Boid::getBoundaryNormalAndDistance(glm::vec3& pos) {
    glm::vec3 normal(0.0f);
    float distance = std::numeric_limits<float>::max();

    const float boundaryThreshold = 1.0f;
    const float cubeMin = -10.0f;
    const float cubeMax = 10.0f;

    if (pos.x < cubeMin + boundaryThreshold) {
        normal.x = 1.0f;
        distance = std::min(distance, pos.x - cubeMin);
    }
    if (pos.x > cubeMax - boundaryThreshold) {
        normal.x = -1.0f;
        distance = std::min(distance, cubeMax - pos.x);
    }

    if (pos.y < cubeMin + boundaryThreshold) {
        normal.y = 1.0f;
        distance = std::min(distance, pos.y - cubeMin);
    }
    if (pos.y > cubeMax - boundaryThreshold) {
        normal.y = -1.0f;
        distance = std::min(distance, cubeMax - pos.y);
    }

    if (pos.z < cubeMin + boundaryThreshold) {
        normal.z = 1.0f;
        distance = std::min(distance, pos.z - cubeMin);
    }
    if (pos.z > cubeMax - boundaryThreshold) {
        normal.z = -1.0f;
        distance = std::min(distance, cubeMax - pos.z);
    }

    return glm::vec4(normal, distance);
}

void Boid::applyForce(glm::vec3 force) {
    acceleration += force;
}

glm::vec3 Boid::alignment(const std::vector<Boid>& boids, float neighborDist) {
    glm::vec3 steering(0.0f);
    int total = 0;             // number of neighbors

    for (const Boid& other : boids) {
        if (other.groupID == groupID) {
            float distance = glm::distance(position, other.position);
            if (&other != this && distance < neighborDist) { // Check if within perception radius
                steering += other.velocity; // Add neighbor's velocity
                total++;
            }
        }      
    }

    if (total > 0) {
        steering /= static_cast<float>(total); // average velocity
        steering = glm::normalize(steering) * maxSpeed; // match speed
        steering -= velocity; // Steer towards average
        if (glm::length(steering) > maxForce) {
            steering = glm::normalize(steering) * maxSpeed * 0.5f; // Limit force
        }
    }
    return steering;
}


glm::vec3 Boid::cohesion(const std::vector<Boid>& boids, float neighborDist) {
    glm::vec3 centerOfMass(0.0f); // store the average position of neighbors
    int total = 0;               // count of neighbors

    for (const Boid& other : boids) {
        if (other.groupID == groupID) {
            float distance = glm::distance(position, other.position); //distance between 2 boys

            // if boys are close to each other, sum up their position and their total
            if (&other != this && distance < neighborDist) {
                centerOfMass += other.position;
                total++;
            }
        }
    }

    if (total > 0) {
        centerOfMass /= static_cast<float>(total); // average position of neighbors
        glm::vec3 desired = centerOfMass - position; // vector toward the center


        if (glm::length(desired) > 0.2f) {
            desired = glm::normalize(desired) * (maxSpeed/3);
        }
        else {
            return glm::vec3(0.0f); // no need to move if already at the center
        }

        // calculate steering force (desired velocity - current velocity)
        glm::vec3 steering = desired - velocity;
        if (glm::length(steering) > maxForce) {
            steering = glm::normalize(steering) * maxForce; // Limit steering force
        }

        return steering;
    }

    // no boys, no cohesion force
    return glm::vec3(0.0f);
}


glm::vec3 Boid::separation(const std::vector<Boid>& boids, float desiredSeparation) {
    glm::vec3 steer(0.0f);
    int total = 0;

    for (const Boid& other : boids) {
        if (other.groupID == groupID) {
            float distance = glm::distance(position, other.position);
            if (&other != this && distance < desiredSeparation) {
                glm::vec3 diff = position - other.position;
                diff = glm::normalize(diff) / (distance * distance); // weight by distance (closer = stronger force)
                steer += diff;
                total++;
            }
        }
    }

    if (total > 0) {
        steer /= static_cast<float>(total);
    }

    if (glm::length(steer) > 0) {
        steer = glm::normalize(steer) * maxSpeed - velocity; // Steering = desired - current velocity
        if (glm::length(steer) > maxForce) {
            steer = glm::normalize(steer) * maxForce; // Limit steering force
        }
    }

    return steer;
}


