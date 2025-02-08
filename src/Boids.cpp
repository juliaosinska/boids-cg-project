#include "Boids.h"
#include "Column.h"


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
    color(color),
    hasCollided(false)
{
    obb.center = position;

    obb.axes[0] = glm::vec3(1.0f, 0.0f, 0.0f); // X-axis
    obb.axes[1] = glm::vec3(0.0f, 1.0f, 0.0f); // Y-axis
    obb.axes[2] = glm::vec3(0.0f, 0.0f, 1.0f); // Z-axix

    obb.halfExtents = glm::vec3(0.6f, 0.3f, 0.2f); // this is the size of our fish hitbox
    //could add something dynamic here mayhaps?
}

void Boid::update(const std::vector<Boid>& boids, float deltaTime, const std::vector<Column>& columns, float alignWeight, float cohesionWeight, float separationWeight, float horizontalBiasStrength) {
    // define cube boundaries - if we want a bigger aquarium, gotta change it here
    const float boundaryThreshold = 1.0f;
    const float cubeMin = -10.0f;
    const float cubeMax = 10.0f;

    // Define weights for the behaviors
    //float alignWeight = 0.4; //zmniejszone sprawia ze ryby w malych stadach tak dziko nie wibruj¹
    //float cohesionWeight = 0.03f;
    //float separationWeight = 0.4f;

    float wallAvoidanceWeight = 0.5f; 

    glm::vec3 alignForce = alignment(boids, perceptionRadius) * alignWeight;
    glm::vec3 cohesionForce = cohesion(boids, perceptionRadius) * cohesionWeight;
    glm::vec3 separationForce = separation(boids, 0.7f) * separationWeight; // the allowed distance for boids to be in
    
    alignForce.y *= 1.2f;       
    cohesionForce.y *= 10.9f;   // fighting the fish columns !!!!!!!!!!
    separationForce.y *= 1.4f; 

    //float horizontalBiasStrength = 0.1f; // Adjust this value to control the strength of the bias
    glm::vec3 horizontalBiasForce = glm::vec3(0.0f, -velocity.y * horizontalBiasStrength, 0.0f);

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
    applyForce(separationForce); // jesli to sobie zakomentujcie to mozna zobaczyc ze hitboxy kinda dzialaja?
    applyForce(horizontalBiasForce);
    
    // make the velocity a bit smooter - the fish wont snap so hard anymore :(

    if (glm::length(acceleration) > maxForce) {
        acceleration = glm::normalize(acceleration) * maxForce;
    }

    velocity = glm::mix(velocity, velocity + acceleration, 0.2f); 
   
    if (glm::length(velocity) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    
    position += velocity ;
    
    //if a fish manages to go through our wall avoidence, this will stop them from popping out of the box
    if (position.x < cubeMin) position.x = cubeMin + boundaryThreshold - 0.8f;
    if (position.x > cubeMax) position.x = cubeMax - boundaryThreshold + 0.8f;
    if (position.y < cubeMin) position.y = cubeMin + boundaryThreshold - 0.8f;
    if (position.y > cubeMax) position.y = cubeMax - boundaryThreshold + 0.8f;
    if (position.z < cubeMin) position.z = cubeMin + boundaryThreshold - 0.8f;
    if (position.z > cubeMax) position.z = cubeMax - boundaryThreshold + 0.8f;

    

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

void  Boid::handleCollision(Boid& boid1, Boid& boid2) {
    //vector between the two boids
    glm::vec3 collisionNormal = glm::normalize(boid1.position - boid2.position);

    if (glm::length(collisionNormal) < 1e-6f) {
        std::cout << "bad!";
        return;
    }
    glm::vec3 relativeVelocity = boid1.velocity - boid2.velocity;
    float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

    if(velocityAlongNormal > 0) return; // this means the boids are already moving apart, so everything good

    float e = 0.2f; // Elasticity (bounce factor). 1 is a perfect bounce, less than 1 is inelastic.
    float j = -(1 + e) * velocityAlongNormal;

    // Apply the impulse to each boid (basic response without mass for simplicity)
    glm::vec3 impulse = j * collisionNormal;

    boid1.velocity += impulse;

    // Apply the opposite impulse to boid2 (push it away from boid1)
   boid2.velocity -= impulse;

    boid1.hasCollided = true;
    boid2.hasCollided = true;
}

void Boid::handleCollisionWithColumn(Boid& boid, const Column& column) {
    // calculate the dir to the column
    glm::vec3 dirToColumn = position - column.position;
    float distToColumn = glm::length(dirToColumn);

    // check if the boid is too close to the column along any axis
    if (distToColumn < glm::length(column.obb.halfExtents) * 1.1f) {
        // Relative position of the boid in the column's local space
        glm::vec3 localPos = position - column.position;

        // Project the boid's position onto the column's axes
        glm::vec3 projection;
        for (int i = 0; i < 3; ++i) {
            projection[i] = glm::dot(localPos, column.obb.axes[i]);
        }

        // calculate overlap in each axis
        glm::vec3 overlap = glm::vec3(0.0f);
        for (int i = 0; i < 3; ++i) {
            if (std::abs(projection[i]) > column.obb.halfExtents[i]) {
                overlap[i] = (std::abs(projection[i]) - column.obb.halfExtents[i]);
            }
        }

        // If there is overlap along any axis, push the boid away
        if (glm::length(overlap) > 0.0f) {
            // Calculate the horizontal push direction (ignore Y-axis)
            glm::vec3 horizontalPushDirection = glm::normalize(glm::vec3(dirToColumn.x, 0.0f, dirToColumn.z));
            if (glm::length(horizontalPushDirection) < 0.01f) {
                horizontalPushDirection = glm::vec3(1.0f, 0.0f, 0.0f); // Default to an arbitrary X direction
            }

            glm::vec3 pushForce = horizontalPushDirection * glm::length(overlap) * 1.8f;

            // Add a small upward force if the fish is too close to the floor
            if (position.y < column.position.y - column.obb.halfExtents.y + 1.0f) {
                pushForce.y = 3.0f;

                // Apply the push force
                applyForce(pushForce);

                velocity = glm::normalize(velocity) * maxSpeed;
            }
        }
    }
}

//void Boid::handleCollisionWithColumn(Boid& boid, const Column& column) {
//    // Calculate the direction to the column
//    glm::vec3 dirToColumn = position - column.position;
//    float distToColumn = glm::length(dirToColumn);
//
//    // check if the boid is too close to the column along any axis
//    if (distToColumn < glm::length(column.obb.halfExtents) * 1.1f) {
//        // relative position of the boid in the column's local space
//        glm::vec3 localPos = position - column.position;
//
//        // Project the boid's position onto the column's axes
//        glm::vec3 projection;
//        for (int i = 0; i < 3; ++i) {
//            projection[i] = glm::dot(localPos, column.obb.axes[i]);
//        }
//
//        // we must check for overlap in diff axes
//        glm::vec3 overlap = glm::vec3(0.0f);
//        for (int i = 0; i < 3; ++i) {
//            if (std::abs(projection[i]) > column.obb.halfExtents[i]) {
//                overlap[i] = (std::abs(projection[i]) - column.obb.halfExtents[i]);
//            }
//        }
//
//        // if there is overlap along any axis, push the boid away !
//        if (glm::length(overlap) > 0.0f) {
//            glm::vec3 pushDirection = glm::normalize(dirToColumn); // Direction away from the column
//
//            glm::vec3 horizontalPushDirection = glm::normalize(glm::vec3(dirToColumn.x, 0.0f, dirToColumn.z));
//            if (glm::length(horizontalPushDirection) < 0.01f) {
//                horizontalPushDirection = glm::vec3(1.0f, 0.0f, 0.0f); // Default to an arbitrary X direction
//            }
//
//            glm::vec3 pushForce = horizontalPushDirection * glm::length(overlap) * 1.8f;
//
//            // Prevent boids from vibrating near the floor by ensuring they don’t get pushed downward
//            if (position.y < column.position.y - column.obb.halfExtents.y + 1.0f) {
//                pushForce.y = 3.0f; // Small upward push to prevent floor sticking
//            }
//
//            //if (std::abs(overlap.y) > 0.0f) {
//            //    // Prefer to push the boid around the column on the X-Z plane rather than up or down
//            //    glm::vec3 horizontalPushDirection = glm::normalize(glm::vec3(dirToColumn.x, 0.0f, dirToColumn.z)); // Ignore Y-axis
//            //    pushDirection = horizontalPushDirection;  // Redirect to X-Z plane
//            //}
//
//            // apply force to push the boid out along the axis with the most overlap
//            //glm::vec3 pushForce = pushDirection * glm::length(overlap) * 1.8f;  
//            applyForce(pushForce);
//
//            // change velocity to prevent fish from quickly re-entering the column
//            velocity = glm::normalize(velocity) * maxSpeed;
//        }
//    }
//}

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


