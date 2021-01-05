#version 150 core

precision highp float;
precision highp int;
layout(std140, column_major) uniform;
layout(points) in;

#define RENDER_QUAD

#ifdef RENDER_QUAD
layout(triangle_strip, max_vertices = 4) out;
#else
layout(points, max_vertices = 4) out;
#endif

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 CameraPosition;

in block
{
	vec4 Color;
} In[];

out block
{
	vec4 Color;
} Out;

void main()
{
	Out.Color = In[0].Color;

#ifdef RENDER_QUAD
/*
	vec3 Center = gl_in[0].gl_Position.xyz;

	vec3 planeNormal = normalize(Center - vec3(MV * vec4(CameraPosition, 1.0)));
	vec3 up = vec3(0.0, 0.0, 1.0);
	vec3 planeTangent = normalize(cross(planeNormal, up));

	vec3 planeUp = normalize(cross(planeNormal, planeTangent));

	vec3 halfTangent = planeTangent * 0.5;
	vec3 halfUp = planeUp * 0.5;

	gl_Position = MVP * vec4(Center - halfTangent - halfUp, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center + halfTangent - halfUp, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center - halfTangent + halfUp, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center + halfTangent + halfUp, 1.0);
	EmitVertex();
*/

	vec3 Center = gl_in[0].gl_Position.xyz;

	vec3 zAxis = vec3(MV * vec4(normalize(Center - CameraPosition), 1.0));
	vec3 yAxis = vec3(0.0, 1.0, 0.0);
	vec3 xAxis = normalize(cross(zAxis, yAxis));

	yAxis = normalize(cross(xAxis, zAxis));

	vec3 x = xAxis * 0.5;
	vec3 y = yAxis * 0.5;

	gl_Position = MVP * vec4(Center - x - y, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center + x - y, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center - x + y, 1.0);
	EmitVertex();

	gl_Position = MVP * vec4(Center + x + y, 1.0);
	EmitVertex();

#else
	gl_Position = MVP * gl_in[0].gl_Position;
	gl_PointSize = gl_in[0].gl_PointSize;
	EmitVertex();
#endif

	EndPrimitive();
}

