#version 410 core
// From Vertex Buffer Object (VBO)
// The only thing that can come 'in', that is
// what our shader reads, the first part of the
// graphics pipeline.
layout(location=0) in vec3 position;

// Uniform variables
uniform mat4 u_Projection; // We'll use a perspective projection

void main()
{
    vec4 newPosition = u_Projection* vec4(position,1.0f);
                                                                    // Don't forget 'w'
  	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
//  	gl_Position = vec4(position.x, position.y, position.z, 1.0f);
}

