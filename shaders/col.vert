#version 430 core

precision highp float;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 view;
uniform mat4 modelMatrix;
uniform mat4 camMatrix;
uniform vec3 scale; 

out vec2 fragTexCoord;
out vec3 fragPosition;
out mat3 TBN;

void main()
{
	// position in world space
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1.0);
    fragPosition = worldPosition.xyz;

    // tangent space (TBN matrix)
    vec3 T = normalize(mat3(modelMatrix) * vertexTangent);
    vec3 N = normalize(mat3(transpose(inverse(modelMatrix))) * vertexNormal);
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    // texture coordinates
    fragTexCoord = vertexTexCoord * scale.xy;;

    // position (clip-space)
    gl_Position = camMatrix * view * worldPosition;
}
