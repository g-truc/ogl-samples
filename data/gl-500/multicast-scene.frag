#version 430 core

#define FRAG_COLOR		0

in block
{
	vec4 Color1;
} In;

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

void main()
{
	Color = In.Color1;
}

