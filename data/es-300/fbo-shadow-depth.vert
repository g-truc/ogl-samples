#version 300 es
precision highp float;

uniform mat4 MVP;

in vec3 Position;

void main()
{
	gl_Position = MVP * vec4(Position, 1.0);
}
