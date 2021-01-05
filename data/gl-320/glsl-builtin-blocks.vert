#version 150 core

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform transform
{
	mat4 MVP;
} Transform;

in vec2 Position;
in vec2 Texcoord;

out block
{
	vec2 Texcoord;
	flat int Instance;
} Out;

void main()
{	
	Out.Texcoord = Texcoord;
	Out.Instance = 0;
	gl_Position = Transform.MVP * vec4(Position, 0.0, 1.0);
}
