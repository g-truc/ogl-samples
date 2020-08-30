#version 300 es

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform highp sampler2D Diffuse;

in vec2 FragTexcoord;

out vec4 Color;

void main()
{
	Color = texture(Diffuse, FragTexcoord);
}
