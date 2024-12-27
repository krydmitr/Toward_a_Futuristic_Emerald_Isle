// #version 330 core
// 
// // Input attributes
// layout(location = 0) in vec3 aPos;      // Vertex position
// layout(location = 1) in vec2 aTexCoord; // Texture coordinates
// 
// // Output to the fragment shader
// out vec2 TexCoord;
// 
// uniform mat4 mvp; // Model-View-Projection matrix
// 
// void main() {
//     gl_Position = mvp * vec4(aPos, 1.0); // Transform the vertex position
//     TexCoord = aTexCoord;               // Pass texture coordinates to the fragment shader
// }






//   #version 330 core
//   layout(location = 0) in vec3 aPos;
//   layout(location = 1) in vec2 aTexCoord;
//   
//   out vec2 TexCoord;
//   
//   uniform mat4 mvp;
//   
//   void main() {
//       gl_Position = mvp * vec4(aPos, 1.0);
//       TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
//   }





#version 330 core

// Input attributes
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

// Outputs to the fragment shader
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 modelMatrix;

void main()
{
    // Transform the vertex to clip space
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Pass world position and normal to the fragment shader
    worldPosition = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
    worldNormal = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);

    // Pass the texture coordinates
    TexCoord = vertexUV;
}
