#version 410 core

layout(location=0) in vec3 position;
//layout(location=1) in vec3 vertexColors; // TODO Uncomment

//out vec3 v_vertexColors; 					// TODO Uncomment 

void main()
{
   // v_vertexColors = vertexColors; // TODO Uncomment

	 gl_Position = vec4(position.x, position.y, position.z, 1.0f);
}

