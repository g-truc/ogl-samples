#version 400 core

#define POSITION	0

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform transform
{
	mat4 MVP;
	mat4 MV;
	vec3 Camera;
} Transform;

const vec3 ConstView = vec3(0, 0,-1);
const vec3 ConstNormal = vec3(0, 0, 1);

layout(location = POSITION) in vec2 Position;

out block
{
	vec3 Reflect;
} Out;

void main()
{	
	mat3 MV3x3 = mat3(Transform.MV);

	gl_Position = Transform.MVP * vec4(Position, 0.0, 1.0);
	vec3 P = MV3x3 * vec3(Position, 0.0);
	vec3 N = MV3x3 * ConstNormal;
	vec3 E = normalize(P - Transform.Camera);
	
	Out.Reflect = reflect(E, N);
}
