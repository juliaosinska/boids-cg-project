#version 430 core

precision highp float;

in vec3 fragPosition;
in vec2 fragTexCoord;
in mat3 TBN;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 cameraPos; 
uniform vec3 objectColor;

uniform sampler2D fishNormalMap;
uniform sampler2D fishTexture;
uniform int useNormalMapping;

out vec4 out_color;

void main()
{
    vec2 fragTexCoord = fragTexCoord * 10.0;

    // color texture
    vec4 textureColor = texture(fishTexture, fragTexCoord);

    // normal mapping
    vec3 N;
    if (useNormalMapping == 1) {
        vec3 normalMapValue = texture(fishNormalMap, fragTexCoord).rgb;
        N = normalize(TBN * (2.0 * normalMapValue - 1.0)); // Apply normal mapping
    } else {
        N = normalize(TBN[2]); // Use the original vertex normal
    }

	// vector normalization
    vec3 L = normalize(lightPos - fragPosition);
    vec3 V = normalize(cameraPos - fragPosition);
    vec3 R = reflect(-L, N);

    // ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse lighting
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular lighting
    float shininess = 8000.0;
    float spec = pow(max(dot(V, R), 0.0), shininess);
    vec3 specular = spec * lightColor;

    // attenuation
    float distance = length(lightPos - fragPosition);
    float attenuation = 1.0 / (distance * distance);

	// final color
    vec3 finalColor = (diffuse + specular + ambient) * attenuation * textureColor.rgb;
    out_color = vec4(finalColor, textureColor.a);
    //out_color = vec4(normalMapValue, 1.0);
    //out_color = vec4(textureColor, 1.0);
}