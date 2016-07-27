#version 430 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 Texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{	
	gl_Position = vec4(4.f * (gl_VertexID % 2) - 1.f, 
					   4.f * (gl_VertexID / 2) - 1.f, 0.0, 1.0);
}

