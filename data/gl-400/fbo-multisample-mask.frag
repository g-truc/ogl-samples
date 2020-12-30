#version 400 core

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform sampler2D Diffuse;

in block
{
	vec2 Texcoord;
} In;

out vec4 Color;

void main()
{
	gl_SampleMask[0] = 0x7;
	Color = texture(Diffuse, In.Texcoord);
}
