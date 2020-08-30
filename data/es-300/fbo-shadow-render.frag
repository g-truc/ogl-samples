#version 300 es

precision highp float;

uniform highp sampler2DShadow Shadow;

in vec4 VertexColor;
in vec4 ShadowCoord;

out highp vec4 FragColor;

void main()
{
	vec3 Coord = ShadowCoord.xyz;
	Coord.z -= 0.005;

	float Visibility = mix(0.5, 1.0, texture(Shadow, Coord));
	FragColor = Visibility * VertexColor;
}
