#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;

out vec3 v_normal;
out vec3 v_fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    v_normal = mat3(transpose(inverse(model))) * normal;
    v_fragPos = vec3(model * vec4(position, 1.0));

    gl_Position = projection * view * model * vec4(position, 1.0);
}
