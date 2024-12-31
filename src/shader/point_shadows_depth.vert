// #version 330 core
// layout (location = 0) in vec3 aPos;
// 
// uniform mat4 model;
// 
// void main()
// {
//     gl_Position = model * vec4(aPos, 1.0);
// }






#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 3) in mat4 instanceMatrix; // Instance matrix (from instancing VBO)

void main()
{
    // Combine the model matrix with the instance-specific transformation
    gl_Position = instanceMatrix * vec4(aPos, 1.0);
}
