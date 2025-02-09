#ifndef VAO_CLASS_H
#define VAO_CLASS_H
#define GLM_ENABLE_EXPERIMENTAL
#include<glad.h>
#include"VBO.h"

class VAO
{
public:
	// ID reference for the VAO
	GLuint ID;
	// constructor that generates a VAO ID
	VAO();

	// links a VBO Attribute such as a position or color to the VAO
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	// binds the VAO
	void Bind();
	// unbinds the VAO
	void Unbind();
	// deletes the VAO
	void Delete();
};

#endif
