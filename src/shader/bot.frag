// #version 330 core
// 
// in vec3 worldPosition;
// in vec3 worldNormal; 
// 
// in vec2 UV;
// 
// in vec2 vTexCoord;
// 
// out vec3 finalColor;
// 
// uniform vec3 lightPosition;
// uniform vec3 lightIntensity;
// 
// uniform sampler2D myTextureSampler;
// 
// void main()
// {
// 	// Lighting
// 	vec3 lightDir = lightPosition - worldPosition;
// 	float lightDist = dot(lightDir, lightDir);
// 	lightDir = normalize(lightDir);
// 	vec3 v = lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0) / lightDist;
// 
// 	// Tone mapping
// 	v = v / (1.0 + v);
// 
// 	// Gamma correction
// 	finalColor = pow(v, vec3(1.0 / 2.2));
// 
// 	// Sample the texture using UV coordinates
//     vec4 textureColor = texture(myTextureSampler, UV);
// 
//     // Combine the texture color with the lighting
//     vec3 finalColor = v * textureColor.rgb;
// 
//     // Output the final color with an alpha of 1.0
//     // fragColor = vec4(finalColor, 1.0);
// }










// #version 330 core
// 
// in vec3 worldPosition;
// in vec3 worldNormal;
// in vec2 UV;                   // <- This is your UV input from the vertex shader
// 
// out vec4 fragColor;           // <- The final output is a four-component color
// 
// uniform vec3 lightPosition;
// uniform vec3 lightIntensity;
// uniform sampler2D myTextureSampler;
// 
// void main()
// {
//     // 1. Compute basic lighting using the normal and a directional light:
//     vec3 lightDir = lightPosition - worldPosition;
//     float lightDist = dot(lightDir, lightDir);
//     lightDir = normalize(lightDir);
// 
//     // Simple diffuse shading: (lightIntensity * cosTheta) / distance
//     float cosTheta = max(dot(lightDir, worldNormal), 0.0);
//     vec3 lightingColor = lightIntensity * cosTheta / lightDist;
// 
//     // 2. Tone mapping (classic simple operator)
//     lightingColor = lightingColor / (1.0 + lightingColor);
// 
//     // 3. Gamma correction
//     lightingColor = pow(lightingColor, vec3(1.0 / 2.2));
// 
//     // 4. Sample the texture using your UV coordinates
//     vec4 textureColor = texture(myTextureSampler, UV);
// 
//     // 5. Combine the texture color with the lighting
//     //    For example, multiply them:
//     vec3 finalRGB = lightingColor * textureColor.rgb;
// 
//     // 6. Output the final color
//     //    We use vec4 because that's typical for fragment output
//     fragColor = vec4(finalRGB, 1.0);
// }













// #version 330 core
// 
// // Inputs from the vertex shader
// in vec3 worldPosition;
// in vec3 worldNormal;
// in vec2 UV;
// 
// // Output color
// out vec4 fragColor;
// 
// // Uniforms
// uniform vec3 lightPosition;      // Position of the light in world space
// uniform vec3 lightIntensity;     // Intensity/color of the light
// uniform vec3 viewPosition;       // Position of the camera/viewer in world space
// uniform sampler2D myTextureSampler; // Texture sampler
// 
// void main()
// {
//     // 1. Compute the light direction and distance
//     vec3 lightDir = lightPosition - worldPosition;
//     float lightDist = length(lightDir);
//     lightDir = normalize(lightDir);
// 
//     // 2. Compute the view direction for specular reflection
//     vec3 viewDir = normalize(viewPosition - worldPosition);
// 
//     // 3. Normalize the interpolated normal
//     vec3 normal = normalize(worldNormal);
// 
//     // 4. Diffuse shading: Lambertian reflection
//     float cosTheta = max(dot(normal, lightDir), 0.0);
//     vec3 diffuse = (lightIntensity * cosTheta) / (lightDist * lightDist);
// 
//     // 5. Specular shading: Blinn-Phong reflection model
//     vec3 halfVector = normalize(lightDir + viewDir);
//     float specAngle = max(dot(normal, halfVector), 0.0);
//     float shininess = 32.0; // Adjust shininess for glossy or matte surfaces
//     vec3 specular = lightIntensity * pow(specAngle, shininess) / (lightDist * lightDist);
// 
//     // 6. Combine lighting components
//     vec3 lightingColor = diffuse + specular;
// 
//     // 7. Tone mapping to compress HDR lighting
//     lightingColor = lightingColor / (1.0 + lightingColor);
// 
//     // 8. Gamma correction for final output
//     lightingColor = pow(lightingColor, vec3(1.0 / 2.2));
// 
//     // 9. Sample the texture
//     vec4 textureColor = texture(myTextureSampler, UV);
// 
//     // 10. Combine texture with lighting
//     vec3 finalRGB = lightingColor * textureColor.rgb;
// 
//     // 11. Output the final color
//     fragColor = vec4(finalRGB, textureColor.a); // Preserve texture alpha
//     
// }




#version 330 core
in vec3 FragPos;
in vec3 NormalInterp;
in vec2 TexCoordInterp;

uniform sampler2D myTextureSampler;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

uniform vec3 lightColor;

out vec4 FragColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightIntensity;

    // Diffuse 
    vec3 norm = normalize(NormalInterp);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightIntensity;

    // Combine results
    vec3 result = (ambient + diffuse) * texture(myTextureSampler, TexCoordInterp).rgb;
    FragColor = vec4(result, 1.0);
}







