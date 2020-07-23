#version 300 es
#define FRAG_COLOR	0

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

flat in highp vec4 FragColor;

layout(location = FRAG_COLOR) out vec4 Color;

void main()
{
	Color = FragColor;
}
