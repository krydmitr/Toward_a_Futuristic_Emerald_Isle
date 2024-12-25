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










#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec2 UV;                   // <- This is your UV input from the vertex shader

out vec4 fragColor;           // <- The final output is a four-component color

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D myTextureSampler;

void main()
{
    // 1. Compute basic lighting using the normal and a directional light:
    vec3 lightDir = lightPosition - worldPosition;
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);

    // Simple diffuse shading: (lightIntensity * cosTheta) / distance
    float cosTheta = max(dot(lightDir, worldNormal), 0.0);
    vec3 lightingColor = lightIntensity * cosTheta / lightDist;

    // 2. Tone mapping (classic simple operator)
    lightingColor = lightingColor / (1.0 + lightingColor);

    // 3. Gamma correction
    lightingColor = pow(lightingColor, vec3(1.0 / 2.2));

    // 4. Sample the texture using your UV coordinates
    vec4 textureColor = texture(myTextureSampler, UV);

    // 5. Combine the texture color with the lighting
    //    For example, multiply them:
    vec3 finalRGB = lightingColor * textureColor.rgb;

    // 6. Output the final color
    //    We use vec4 because that's typical for fragment output
    fragColor = vec4(finalRGB, 1.0);
}


