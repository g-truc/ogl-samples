#version 300 es

precision highp float;

uniform mat4 MVP;
uniform mat4 DepthBiasMVP;

in vec3 Position;
in vec4 Color;

out vec4 VertexColor;
out vec4 ShadowCoord;

void main()
{
	gl_Position = MVP * vec4(Position, 1.0);
	ShadowCoord = DepthBiasMVP * vec4(Position, 1.0);
	VertexColor = Color;
}
