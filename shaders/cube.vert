#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camMatrix;

void main() {
    gl_Position = camMatrix  * view * model * vec4(aPos, 1.0);
}
