#version 150 core

precision highp float;
precision highp int;
layout(std140, column_major) uniform;

out vec4 Color;

in block
{
	float Angle;
	float Opacity;
} In;

vec3 rgbColor(vec3 hsvColor)
{
	vec3 hsv = hsvColor;
	vec3 rgbColor;

	if(hsv.y == 0)
		// achromatic (grey)
		rgbColor = vec3(hsv.z);
	else
	{
		float sector = floor(hsv.x / float(60));
		float frac = (hsv.x / float(60)) - sector;
		// factorial part of h
		float o = hsv.z * (float(1) - hsv.y);
		float p = hsv.z * (float(1) - hsv.y * frac);
		float q = hsv.z * (float(1) - hsv.y * (float(1) - frac));

		switch(int(sector))
		{
		default:
		case 0:
			rgbColor.r = hsv.z;
			rgbColor.g = q;
			rgbColor.b = o;
			break;
		case 1:
			rgbColor.r = p;
			rgbColor.g = hsv.z;
			rgbColor.b = o;
			break;
		case 2:
			rgbColor.r = o;
			rgbColor.g = hsv.z;
			rgbColor.b = q;
			break;
		case 3:
			rgbColor.r = o;
			rgbColor.g = p;
			rgbColor.b = hsv.z;
			break;
		case 4:
			rgbColor.r = q; 
			rgbColor.g = o; 
			rgbColor.b = hsv.z;
			break;
		case 5:
			rgbColor.r = hsv.z; 
			rgbColor.g = o; 
			rgbColor.b = p;
			break;
		}
	}

	return rgbColor;
}

void main()
{
/*
	float Size = length(gl_PointCoord - 0.5);
	if(Size > 0.5)
		discard;

	Color.rgb = vec3(1.0, 1.0, 1.0);
	Color.a = smoothstep(0.0, 1.0, 1.0 - Size * 2.0) * 0.01;
*/

	Color.rgb = rgbColor(vec3(In.Angle / 6.28318530718 * 360.0, 1.0f, 1.0f));

	float Size = length(gl_PointCoord - 0.5);
	Color.a = (1.0 - Size * 2.0) * In.Opacity;
}



