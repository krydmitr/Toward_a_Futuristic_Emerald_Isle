#version 330 core

// Input attributes
layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec2 aTexCoord; // Texture coordinates

// Output to the fragment shader
out vec2 TexCoord;

uniform mat4 mvp; // Model-View-Projection matrix

void main() {
    gl_Position = mvp * vec4(aPos, 1.0); // Transform the vertex position
    TexCoord = aTexCoord;               // Pass texture coordinates to the fragment shader
}
