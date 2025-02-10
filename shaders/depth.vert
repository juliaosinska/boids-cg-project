#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightProjection;
uniform mat4 model;
uniform mat4 view;

void main()
{
    gl_Position = lightProjection * view * model * vec4(aPos, 1.0);
}