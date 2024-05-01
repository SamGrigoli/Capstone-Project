#version 410 core

//My diffrent attributes
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColors;

uniform mat4 u_ModelMatrix;
uniform mat4 u_ModelMatrix2;
uniform mat4 u_ModelMatrix3;
uniform mat4 u_ModelMatrix4;
uniform mat4 u_ModelMatrix5;

//uniform float u_offset; //Uniform variable

out vec3 v_vertexColors;


void main()
{
	v_vertexColors = vertexColors;
	//vec4 newPosition = u_ModelMatrix * vec4(position, 1.0f);

	 if (gl_VertexID < 4) { // Assuming each quad has 6 vertices
        gl_Position = u_ModelMatrix * vec4(position, 1.0f);
    } else if (gl_VertexID >= 16 && gl_VertexID < 20){
		gl_Position = u_ModelMatrix3 * vec4(position, 1.0f);
	}else if (gl_VertexID >= 4 && gl_VertexID < 16){ // Apply transformation for quad 2
        gl_Position = u_ModelMatrix2 * vec4(position, 1.0f);
    } else if (gl_VertexID >= 20 && gl_VertexID < 24) {
		gl_Position = u_ModelMatrix4 * vec4(position, 1.0f);
	} else {
	    gl_Position = u_ModelMatrix5 * vec4(position, 1.0f);
	}

};

