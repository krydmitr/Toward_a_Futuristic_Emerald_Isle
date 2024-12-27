// #version 330 core
// 
// // Input from the vertex shader
// in vec2 TexCoord;
// 
// // Output color
// out vec4 FragColor;
// 
// // Uniforms
// uniform sampler2D texture1; // Texture sampler
// uniform bool useTexture;    // Toggle for using texture or solid color
// uniform vec4 solidColor;    // Solid fallback color
// 
// void main() {
//     if (useTexture) {
//         // Use the texture color
//         // FragColor = texture(texture1, TexCoord);
//         FragColor = vec4(0.0, 1.0, 0.0, 1.0); // Solid green
//     } else {
//         // Use the solid color
//         FragColor = solidColor;
//     }
// }











// #version 330 core
// out vec4 FragColor;
// 
// in vec2 TexCoord;
// 
// uniform sampler2D texture1;
// 
// void main() {
//     FragColor = texture(texture1, TexCoord); // Sample the texture
// }







#version 330 core

// Output color
out vec4 FragColor;

// Input from the vertex shader
in vec2 TexCoord;         // Texture coordinates
in vec3 worldPosition;    // Position of the fragment in world space
in vec3 worldNormal;      // Normal vector of the fragment in world space

// Uniforms
uniform sampler2D texture1;    // The texture for the plane
uniform vec3 lightPosition;    // Position of the light in world space
uniform vec3 lightIntensity;   // Intensity/color of the light
uniform vec3 viewPosition;     // Position of the camera/viewer in world space

void main()
{
    vec3 temp = lightPosition + lightIntensity + viewPosition;
    // 1. Compute the light direction and distance
    vec3 lightDir = lightPosition - worldPosition;
    float lightDist = length(lightDir);
    lightDir = normalize(lightDir);

    // 2. Compute the view direction for specular reflection
    vec3 viewDir = normalize(viewPosition - worldPosition);

    // 3. Normalize the interpolated normal
    vec3 normal = normalize(worldNormal);

    // 4. Diffuse shading: Lambertian reflection
    float cosTheta = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = (lightIntensity * cosTheta) / (lightDist * lightDist);

    // 5. Specular shading: Blinn-Phong reflection model
    vec3 halfVector = normalize(lightDir + viewDir);
    float specAngle = max(dot(normal, halfVector), 0.0);
    float shininess = 32.0; // Adjust shininess for glossy or matte surfaces
    vec3 specular = lightIntensity * pow(specAngle, shininess) / (lightDist * lightDist);

    // 6. Combine lighting components
    vec3 lightingColor = diffuse + specular;

    // 7. Tone mapping to compress HDR lighting
    lightingColor = lightingColor / (1.0 + lightingColor);

    // 8. Gamma correction for final output
    lightingColor = pow(lightingColor, vec3(1.0 / 2.2));

    // 9. Sample the texture
    vec4 textureColor = texture(texture1, TexCoord);

    // 10. Combine texture with lighting
    vec3 finalRGB = lightingColor * textureColor.rgb;

    // 11. Output the final color
    FragColor = vec4(finalRGB, textureColor.a); // Preserve texture alpha
    // FragColor = vec4(normalize(worldNormal) * 0.5 + 0.5, 1.0); // Map [-1,1] to [0,1]
    // FragColor = vec4(diffuse, 1.0); // Debug diffuse lighting
    // FragColor = vec4(specular, 1.0); // Debug specular lighting
    // FragColor = texture(texture1, TexCoord);
    // FragColor = vec4(vec3(cosTheta), 1.0);


}


