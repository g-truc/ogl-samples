#version 460 core
#extension GL_NV_fragment_shader_barycentric : enable

#define DIFFUSE			0
#define FRAG_COLOR		0

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

layout(binding = DIFFUSE) uniform sampler2D Diffuse;

pervertexNV in block
{
	vec2 Texcoord;
} In[];

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

void main()
{
	vec2 Texcoord = gl_BaryCoordNV.x * In[0].Texcoord + gl_BaryCoordNV.y * In[1].Texcoord + gl_BaryCoordNV.z * In[2].Texcoord;
	Color = texture(Diffuse, Texcoord);
}
