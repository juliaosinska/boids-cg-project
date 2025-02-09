#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Texture Coordinates
layout (location = 1) in vec2 aTex;
// Normals (not necessarily normalized)
layout (location = 2) in vec3 aNormal;



// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;
// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the current position for the Fragment Shader
out vec3 crntPos;
// Shadow map coordinates
out vec4 FragPosLightSpace;


// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix from the main function
uniform mat4 model;
// Shadow map matrix
uniform mat4 lightSpaceMatrix;

void main()
{
	// calculates current position
	crntPos = vec3(model * vec4(aPos, 1.0f));
	// Outputs the positions/coordinates of all vertices
	gl_Position = camMatrix * vec4(crntPos, 1.0);

	
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
	// Assigns the normal from the Vertex Data to "Normal"
    Normal = mat3(transpose(inverse(model))) * aNormal;

	// Compute shadow coordinates
    FragPosLightSpace = lightSpaceMatrix * vec4(crntPos, 1.0);
}