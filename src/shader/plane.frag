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











#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord); // Sample the texture
}
