#version 330 core

// outputs colors in RGBA
out vec4 FragColor;

// imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// imports the normal from the Vertex Shader
in vec3 Normal;
// imports the current position from the Vertex Shader
in vec3 crntPos;
// shadow map coordinates
in vec4 FragPosLightSpace;

// gets the Texture Unit from the main function
uniform sampler2D tex0;
// gets the color of the light from the main function
uniform vec4 lightColor;
// gets the position of the light from the main function
uniform vec3 lightPos;
// gets the position of the camera from the main function
uniform vec3 camPos;
// shadow map
uniform sampler2D shadowMap;
// for shadow mapping enabling/disabling
uniform int useShadowMapping;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{   
    if (useShadowMapping == 0) {
        return 0.0; // no shadow if shadow mapping is disabled
    }

    // transform from homogeneous coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // convert from [-1,1] to [0,1]

    // get the closest depth from the shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get current fragment depth
    float currentDepth = projCoords.z;
    
    // compare depths and apply bias
    float bias = 0.005f;
    float shadow = currentDepth > closestDepth + bias ? 0.7 : 0.0;
    
    return shadow;
}

void main()
{
    // calculate lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - crntPos);

    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);

    vec3 light = vec3(lightColor) * (1.0 - shadow);
    FragColor = texture(tex0, texCoord) * vec4(light, 1.0);
}
