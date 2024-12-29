//  #version 330 core
//  
//  // Input
//  layout(location = 0) in vec3 vertexPosition;
//  layout(location = 1) in vec3 vertexNormal;
//  layout(location = 2) in vec2 vertexUV;
//  
//  // Output data, to be interpolated for each fragment
//  out vec3 worldPosition;
//  out vec3 worldNormal;
//  out vec2 UV;
//  
//  uniform mat4 MVP;
//  
//  void main() {
//      // Transform vertex
//      gl_Position =  MVP * vec4(vertexPosition, 1.0);
//  
//      // World-space geometry 
//      worldPosition = vertexPosition;
//      worldNormal = vertexNormal;
//  
//      UV = vertexUV;
//  }




















// #version 330 core
// 
// // Input
// layout(location = 0) in vec3 vertexPosition;
// layout(location = 1) in vec3 vertexNormal;
// layout(location = 2) in vec2 vertexUV;
// 
// // Output data, to be interpolated for each fragment
// out vec3 worldPosition;
// out vec3 worldNormal;
// out vec2 UV;
// 
// // Uniforms
// uniform mat4 MVP;          // Model-View-Projection matrix
// uniform mat4 modelMatrix;  // Model transformation matrix
// // uniform mat3 normalMatrix; // Normal transformation matrix
// uniform mat4 normalMatrix; 
// 
// 
// void main() {
//     // Transform vertex position to clip space
//     gl_Position = MVP * vec4(vertexPosition, 1.0);
// 
//     // Transform vertex position to world space
//     worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
// 
//     // Transform vertex normal to world space
//     // worldNormal = normalize(normalMatrix * vertexNormal);
//     worldNormal = normalize((normalMatrix * vec4(vertexNormal, 0.0)).xyz);
// 
//     // Pass UV coordinates to the fragment shader
//     UV = vertexUV;
// }







#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 Model;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

out vec3 FragPos;
out vec3 NormalInterp;
out vec2 TexCoordInterp;

void main()
{
    FragPos = vec3(Model * vec4(Position, 1.0));
    NormalInterp = mat3(transpose(inverse(Model))) * Normal;
    TexCoordInterp = TexCoord;
    gl_Position = MVP * vec4(Position, 1.0);
}








