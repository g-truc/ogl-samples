#version 400 core

#define POSITION	0
#define TRANSFORM0	1
#define TRANSFORM1	2	

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform int Instance;

uniform transform
{
	mat4 MVP;
} Transform[2];

out block
{
	flat int Instance;
} Out;

layout(location = POSITION) in vec2 Position;

void main()
{
	Out.Instance = Instance;
	gl_Position = Transform[Instance].MVP * vec4(Position, 0.0, 1.0);
}
