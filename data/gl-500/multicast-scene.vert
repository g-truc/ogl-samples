#version 430 core

struct SceneData
{	
	vec4 Color;
};

layout(std140,binding = 0) uniform sceneBuffer 
{ 
	SceneData scene; 
}; 

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 Texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out block
{
	vec4 Color1;
} Out;

void main()
{
	Out.Color1 = scene.Color;

	gl_Position = vec4(4.f * (gl_VertexID % 2) - 1.f, 
					4.f * (gl_VertexID / 2) - 1.f, 0.0, 1.0);
}
