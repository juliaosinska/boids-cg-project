#version 330 core

// positions/coordinates
layout (location = 0) in vec3 aPos;
// texture coordinates
layout (location = 2) in vec2 aTex;
// normals (not necessarily normalized)
layout (location = 1) in vec3 aNormal;



// outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;
// outputs the normal for the Fragment Shader
out vec3 Normal;
// outputs the current position for the Fragment Shader
out vec3 crntPos;
// shadow map coordinates
out vec4 FragPosLightSpace;


// imports the camera matrix from the main function
uniform mat4 camMatrix;
// imports the model matrix from the main function
uniform mat4 model;
// shadow map matrix
uniform mat4 lightSpaceMatrix;

uniform vec3 scale;

void main()
{
	// calculates current position
	crntPos = vec3(model * vec4(aPos, 1.0f));
	// outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	
	// assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	// assigns the normal from the Vertex Data to "Normal"
    Normal = mat3(transpose(inverse(model))) * aNormal;

	// compute shadow coordinates
    FragPosLightSpace = lightSpaceMatrix * vec4(crntPos, 1.0);
}