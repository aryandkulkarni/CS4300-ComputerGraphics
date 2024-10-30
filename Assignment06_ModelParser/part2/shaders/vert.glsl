#version 410 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColors;
layout(location = 2) in vec3 vertexNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v_vertexColors;
out vec3 v_vertexNormals;

void main() {
    v_vertexColors = vertexColors;
    v_vertexNormals = vertexNormals;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
