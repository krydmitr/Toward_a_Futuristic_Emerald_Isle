#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
} fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform samplerCube depthMap; // the cube map containing depth
uniform float far_plane;

// A simple function to do the shadow calculation
float ShadowCalculation(vec3 fragPos)
{
    // Vector from light to the fragment
    float currentDist = length(fragPos - lightPos);

    // Sample from our depth cubemap: 
    // The direction is (fragPos - lightPos), the distance stored in .r
    float closestDist = texture(depthMap, fragPos - lightPos).r;
    closestDist *= far_plane;

    // Optional bias to reduce shadow acne
    float bias = 0.05;

    // If the distance from light to this fragment is bigger than 
    // the distance stored in the cubemap => in shadow
    float shadow = (currentDist - bias) > closestDist ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    // Simple shading
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.WorldPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Calculate shadow
    float shadow = ShadowCalculation(fs_in.WorldPos);

    vec3 lighting = vec3(1.0) * diff * (1.0 - shadow);
    FragColor = vec4(lighting, 1.0);
}
