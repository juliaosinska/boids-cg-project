
#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glad.h>


class VBO
{
public:
	// reference ID of the VBO
	GLuint ID;
	// constructor that generates a VBO and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	// binds the VBO
	void Bind();
	// unbinds the VBO
	void Unbind();
	// deletes the VBO
	void Delete();
};

#endif