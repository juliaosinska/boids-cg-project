#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include "glad.h"	


class EBO
{
public:
	// ID reference of EBO
	GLuint ID;
	// constructor that generates a EBO and links it to indices
	EBO(GLuint* indices, GLsizeiptr size);

	// binds the EBO
	void Bind();
	// unbinds the EBO
	void Unbind();
	// deletes the EBO
	void Delete();
};

#endif