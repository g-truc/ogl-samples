#version 460 core

#define POSITION		0
#define TEXCOORD		4

#define TRANSFORM0		1
#define INDIRECTION		3

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

layout(binding = TRANSFORM0) uniform transform
{
	mat4 MVP;
} Transform;

layout(location = POSITION) in vec2 Position;
layout(location = TEXCOORD) in vec2 Texcoord;

out gl_PerVertex
{
	vec4 gl_Position;
};

out block
{
	vec2 Texcoord;
} Out;

void main()
{
	Out.Texcoord = Texcoord;
	gl_Position = Transform.MVP * vec4(Position, 0.0, 1.0);
}
