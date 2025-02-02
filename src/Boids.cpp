#include "Boids.h"


// tutaj mozna cos zdynamizowac, idk dac uzytkownikowi moc zmieniania maxSpeed boidow?
Boid::Boid(glm::vec3 startPosition, glm::vec3 startVelocity, int groupID, glm::vec3 color)
    : position(startPosition),
    velocity(startVelocity),
    acceleration(glm::vec3(0.0f)),
    maxSpeed(0.03f),
    maxForce(0.01f),
    angle(0.0f),
    perceptionRadius(1.5f),
    groupID(groupID),
    color(color)
{}

void Boid::update(const std::vector<Boid>& boids) {

    // Define weights for the behaviors
    float alignWeight = 0.1f; //zmniejszone sprawia ze ryby w malych stadach tak dziko nie wibruj¹
    float cohesionWeight = 0.01f;
    float separationWeight = 2.0f;

    glm::vec3 alignForce = alignment(boids, perceptionRadius) * alignWeight;
    glm::vec3 cohesionForce = cohesion(boids, perceptionRadius) * cohesionWeight;
    glm::vec3 separationForce = separation(boids, 0.5f) * separationWeight;


    applyForce(alignForce);
    if (isNearBoundary(position)) {
        // also preventing clumping
        cohesionForce *= 0.5f;
        // this so they dont clump together as hard when they hit the walls
        separationForce *= 0.5f;
    }
    applyForce(cohesionForce);
    applyForce(separationForce);

    //velocity += acceleration;
    // make the velocity a bit smooter - the fish wont snap so hard anymore :(
    velocity = glm::mix(velocity, velocity + acceleration, 0.2f); 
    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }

    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }

    position += velocity;
    

    // define cube boundaries - if we want a bigger aquarium, gotta change it here
    const float boundaryThreshold = 1.0f;
    const float cubeMin = -10.0f;
    const float cubeMax = 10.0f;

    if (position.x < cubeMin + boundaryThreshold) {
        position.x = cubeMin + boundaryThreshold;
        velocity.x = -velocity.x;
    }
    else if (position.x > cubeMax - boundaryThreshold) {
        position.x = cubeMax - boundaryThreshold;
        velocity.x = -velocity.x;
    }

    if (position.y < cubeMin + boundaryThreshold) {
        position.y = cubeMin + boundaryThreshold;
        velocity.y = -velocity.y;
    }
    else if (position.y > cubeMax - boundaryThreshold) {
        position.y = cubeMax - boundaryThreshold;
        velocity.y = -velocity.y;
    }

    if (position.z < cubeMin + boundaryThreshold) {
        position.z = cubeMin + boundaryThreshold;
        velocity.z = -velocity.z;
    }
    else if (position.z > cubeMax - boundaryThreshold) {
        position.z = cubeMax - boundaryThreshold;
        velocity.z = -velocity.z;
    }

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

bool Boid::isNearBoundary(const glm::vec3& position) {
    const float cubeMin = -10.0f;
    const float cubeMax = 10.0f;
    const float boundaryThreshold = 1.0f; // Distance from boundary to consider "near"

    return (position.x < cubeMin + boundaryThreshold || position.x > cubeMax - boundaryThreshold ||
        position.y < cubeMin + boundaryThreshold || position.y > cubeMax - boundaryThreshold ||
        position.z < cubeMin + boundaryThreshold || position.z > cubeMax - boundaryThreshold);
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
            steering = glm::normalize(steering) * maxForce; // Limit force
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


        if (glm::length(desired) > 0) {
            desired = glm::normalize(desired) * neighborDist;
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
                diff = glm::normalize(diff) / distance; // weight by distance (closer = stronger force)
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


