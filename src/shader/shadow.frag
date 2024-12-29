#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform float farPlane;

uniform samplerCube depthMap;

// Function to calculate shadow factor
float ShadowCalculation(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;

    float currentDepth = length(fragToLight);
    float closestDepth = texture(depthMap, fragToLight).r * farPlane;
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() {
    // Ambient
    vec3 ambient = 0.2 * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;

    // Shadow
    float shadow = ShadowCalculation(FragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    FragColor = vec4(lighting, 1.0);
}
