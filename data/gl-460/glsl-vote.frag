#version 440 core
#extension GL_ARB_shader_group_vote : enable

#define FRAG_COLOR	0
#define DIFFUSE		0

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

layout(binding = DIFFUSE) uniform sampler2D Diffuse;

layout(location = FRAG_COLOR, index = 0) out vec4 Color;

void main()
{
	float Size = length(gl_PointCoord - 0.5);
	if(allInvocationsARB(Size > 0.5))
		discard;

	Color.rgb = texture(Diffuse, gl_PointCoord).rgb;
	Color.a = smoothstep(0.0, 1.0, 1.0 - Size * 2.0);
}

