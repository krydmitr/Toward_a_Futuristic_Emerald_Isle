#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 3) in mat4 instanceMatrix;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
// uniform mat4 model;

uniform bool reverse_normals;

uniform vec2 offsets[100];

void main()
{

    vec2 instanceOffset = offsets[gl_InstanceID];
    vec3 offsetPos = aPos + vec3(instanceOffset, 0.0);
    // vec3 offsetPos = aPos;

    // vec4 worldPosition = model * vec4(offsetPos, 1.0);
    // vs_out.FragPos = vec3(worldPosition);

    vec4 worldPosition = instanceMatrix * vec4(aPos, 1.0);

    // vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(instanceMatrix))) * (-1.0 * aNormal);
    else
        vs_out.Normal = transpose(inverse(mat3(instanceMatrix))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    // gl_Position = projection * view * model * vec4(aPos, 1.0);
    // gl_Position = projection * view * worldPosition;







    
    vs_out.FragPos = vec3(worldPosition);
    gl_Position = projection * view * worldPosition;
}