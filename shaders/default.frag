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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Transform from homogeneous coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Convert from [-1,1] to [0,1]

    // Get the closest depth from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Current depth of the fragment
    float currentDepth = projCoords.z;

    // Shadow factor
    float shadow = currentDepth > closestDepth ? 0.5 : 1.0; // Simple shadow (no PCF)

    return shadow;
}

void main()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	// outputs final color
	// FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient * specular);

	// Compute shadow factor
    float shadow = ShadowCalculation(FragPosLightSpace);

    // outputs final color with shadow applied
    FragColor = texture(tex0, texCoord) * lightColor * (diffuse * shadow + ambient + specular);
}