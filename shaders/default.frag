#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Shadow map coordinates
in vec4 FragPosLightSpace;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;
// Gets the color of the light from the main function
uniform vec4 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;
// Shadow map
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // Transform from homogeneous coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Convert from [-1,1] to [0,1]

    // Get the closest depth from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get current fragment depth
    float currentDepth = projCoords.z;
    
    // Compare depths and apply bias
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    // Calculate lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - crntPos);

    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);

    vec3 light = vec3(lightColor) * (1.0 - shadow);
    FragColor = texture(tex0, texCoord) * vec4(light, 1.0);
}
