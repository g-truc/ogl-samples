precision highp float;

uniform sampler2D Shadow;
uniform vec3 PointLightPosition;
uniform vec2 ShadowClipNearFar;
uniform float Bias;

varying vec3 VertexColor;
varying vec3 VertexPosition;

void main()
{
	vec3 LightNormal = normalize(PointLightPosition - VertexPosition.xyz);

	float FromLightToFrag = (length(VertexPosition.xyz - PointLightPosition) - ShadowClipNearFar.x) / (ShadowClipNearFar.y - ShadowClipNearFar.x);

	float Depth = texture2DProj(Shadow, -LightNormal).x;

	float LightIntensity = 0.5;
	if (Depth + Bias >= FromLightToFrag)
		LightIntensity = 1.0;

	gl_FragColor = vec4(VertexColor * LightIntensity, 1.0);
}
