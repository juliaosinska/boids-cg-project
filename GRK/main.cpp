#include <stdio.h>
#include <glew.h>
#include <freeglut.h>

int main(int argc, char* argv[]) {
	
	printf("Hi\n");
	glutInit(&argc, argv);
	glutCreateWindow("GLEW Test");
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	getchar();
	return 0;
}