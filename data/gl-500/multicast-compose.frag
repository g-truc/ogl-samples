#version 430 core

layout( binding = 0 ) uniform sampler2D tex[2];

struct ComposeData  
{ 
	int in_width;   // width of the input textures 
	int in_height;  // height of the input textures 
	int out_width;  // width of the output buffer 
	int out_height; // height of the output buffer 
}; 

layout(std140,binding=0) uniform composeBuffer { 
	ComposeData compose; 
}; 

uniform sampler2D Diffuse;

in vec4 gl_FragCoord;
out vec4 Color;

void main()
{
	const uint mid = compose.out_width/2;

	uint i; 
	float out_x; 

	if( gl_FragCoord.x < mid ) 
	{ 
		i = 0; 
		out_x = gl_FragCoord.x; 
	} 
	else 
	{ 
		i = 1; 
		out_x = gl_FragCoord.x - mid; 
	} 

	float out_y = gl_FragCoord.y; 
 
	float in_x = compose.in_width  * out_x / (compose.out_width/2); 
	float in_y = compose.in_height * out_y / compose.out_height; 
 
	Color = texelFetch( tex[i], ivec2( in_x, in_y ), 0 ); 
}

