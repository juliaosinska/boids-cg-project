#ifndef PYRAMID_H
#define PYRAMID_H

#include <vector>
#include <glm.hpp>
#include "ShaderClass.h"
#include "Boids.h"

void setupPyramid();
void renderBoids(std::vector<Boid>& boids, Shader& shaderProgram);
void setUpBoids(std::vector<Boid>& boids, int numGroups, int numBoidsPerGroup);
#endif
