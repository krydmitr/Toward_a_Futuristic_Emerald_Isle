#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform samplerCube depthMap; // Depth cubemap
uniform int face;             // Face index to visualize (0 to 5)

void main()
{
    // Map 2D texture coordinates to 3D direction vector based on the selected face
    vec3 direction;
    if (face == 0) // POSITIVE_X
        direction = vec3(1.0, TexCoords.y * 2.0 - 1.0, TexCoords.x * 2.0 - 1.0);
    else if (face == 1) // NEGATIVE_X
        direction = vec3(-1.0, TexCoords.y * 2.0 - 1.0, -TexCoords.x * 2.0 + 1.0);
    else if (face == 2) // POSITIVE_Y
        direction = vec3(TexCoords.x * 2.0 - 1.0, 1.0, -TexCoords.y * 2.0 + 1.0);
    else if (face == 3) // NEGATIVE_Y
        direction = vec3(TexCoords.x * 2.0 - 1.0, -1.0, TexCoords.y * 2.0 - 1.0);
    else if (face == 4) // POSITIVE_Z
        direction = vec3(TexCoords.x * 2.0 - 1.0, TexCoords.y * 2.0 - 1.0, 1.0);
    else if (face == 5) // NEGATIVE_Z
        direction = vec3(-TexCoords.x * 2.0 + 1.0, TexCoords.y * 2.0 - 1.0, -1.0);
    else
        direction = vec3(0.0, 0.0, 0.0); // Default case

    // Sample the depth from the cubemap
    float depth = texture(depthMap, direction).r;

    // Directly use the depth value for visualization
    FragColor = vec4(vec3(depth), 1.0);
}
