#version 150 core

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

uniform sampler2D Diffuse;

in block
{
	vec3 Color;
} In;

out vec4 Color;

void main()
{
	float Size = length(gl_PointCoord - 0.5);
	if(Size > 0.5)
		discard;

	Color.rgb = In.Color * texture(Diffuse, gl_PointCoord).rgb;
	Color.a = smoothstep(0.0, 1.0, 1.0 - Size * 2.0);
}

