#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad.h>
#include<glfw3.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>
#include<gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<gtx/rotate_vector.hpp>
#include<gtx/vector_angle.hpp>


#include"shaderClass.h"

class Camera
{
public:
	// stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	// prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// stores the width and height of the window
	int width;
	int height;

	// adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 10.0f;

	// camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// updates and exports the camera matrix to the vertex shader
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	void Matrix(Shader& shader, const char* uniform);

	glm::mat4 GetViewMatrix() const;
	// handles camera inputs
	void Inputs(GLFWwindow* window);
};
#endif