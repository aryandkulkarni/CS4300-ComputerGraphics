#version 410 core

in vec3 v_vertexColors;
in vec3 v_vertexNormals;

out vec4 outColor;

// Entry point of program
void main() {
    outColor = vec4(v_vertexColors, 1.0);
}
