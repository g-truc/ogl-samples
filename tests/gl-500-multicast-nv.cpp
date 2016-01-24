///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Samples Pack (ogl-samples.g-truc.net)
///
/// Copyright (c) 2004 - 2014 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////

#include "test.hpp"

// bit 0
#define GPUMASK_0 1<<0

// bit 1
#define GPUMASK_1 1<<1

namespace
{
	char const * VERT_SHADER_SOURCE_SCENE("gl-500/multicast-scene.vert");
	char const * FRAG_SHADER_SOURCE_SCENE("gl-500/multicast-scene.frag");

	char const * VERT_SHADER_SOURCE_COMPOSE("gl-500/multicast-compose.vert");
	char const * FRAG_SHADER_SOURCE_COMPOSE("gl-500/multicast-compose.frag");

	GLsizei const VertexCount(6);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f))
	};

	// UBO Data
	struct SceneData
	{
		glm::vec4 Color;
	};

	struct ComposeData
	{
		int in_width;   // width of the input textures 
		int in_height;  // height of the input textures 
		int out_width;  // width of the output buffer 
		int out_height; // height of the output buffer 
	};

	SceneData sceneData;
	ComposeData composeData;
	
	GLuint TextureLeft(0);
	GLuint TextureRight(0);

	GLuint RenderFBO(0);
	GLuint TempRenderFBO(0);

	GLuint ProgramRender(0);
	GLuint ProgramCompose(0);

	GLuint VertexArrayName(0);

	GLuint UBOBuffer;
	GLuint ComposeUBO;

	GLuint VertexBufferName;
}  

class instance : public test
{
public:
	instance(int argc, char* argv[]) :
		test(argc, argv, "gl-500-multicast-nv", test::CORE, 4, 5),
		FramebufferScale(2)
	{}

private:
	glm::uint FramebufferScale;


	bool initProgram()
	{
		// Create Scene Program
		compiler Compiler;
		GLuint VertShaderRender = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_SCENE, "--version 450 --profile core");
		GLuint FragShaderRender = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_SCENE, "--version 450 --profile core");
		bool Validated = Compiler.check();

		ProgramRender = glCreateProgram();
		glProgramParameteri(ProgramRender, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramRender, VertShaderRender);
		glAttachShader(ProgramRender, FragShaderRender);
		glLinkProgram(ProgramRender);

		Validated = Validated && Compiler.checkProgram(ProgramRender);

		// Create Compose Program
		GLuint VertShaderCompose = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_COMPOSE, "--version 450 --profile core");
		GLuint FragShaderCompose = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_COMPOSE, "--version 450 --profile core");
		Validated = Validated && Compiler.check();

		ProgramCompose = glCreateProgram();
		glProgramParameteri(ProgramCompose, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramCompose, VertShaderCompose);
		glAttachShader(ProgramCompose, FragShaderCompose);
		glLinkProgram(ProgramCompose);

		Validated = Validated && Compiler.checkProgram(ProgramCompose);

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(1, &VertexBufferName);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferName);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// GPU MULTICAST - mark UBOs LGPU_SEPARATE_STORAGE_BIT_NVX
		glCreateBuffers(1, &UBOBuffer);
		glNamedBufferStorageEXT(UBOBuffer, sizeof(SceneData), nullptr, GL_DYNAMIC_STORAGE_BIT | LGPU_SEPARATE_STORAGE_BIT_NVX);

		glCreateBuffers(1, &ComposeUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, ComposeUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(ComposeData), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		// Get the Framebuffer Size
		glm::ivec2 const FramebufferSize(this->getWindowSize());

		// Init Left Texture
		glGenTextures(1, &TextureLeft);
		glBindTexture(GL_TEXTURE_2D, TextureLeft);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FramebufferSize.x / 2.0, FramebufferSize.y);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		// Set Multicast on Left Textures
		// clear the Left texture via a FBO once to get a P2P flag 
		glBindFramebuffer(GL_FRAMEBUFFER, TempRenderFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureLeft, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Init Right Texture
		glGenTextures(1, &TextureRight);
		glBindTexture(GL_TEXTURE_2D, TextureRight);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, FramebufferSize.x / 2.0, FramebufferSize.y);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set Multicast on Right Textures
		// clear the Right texture via a FBO once to get a P2P flag 
		glBindFramebuffer(GL_FRAMEBUFFER, TempRenderFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureRight, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f)[0]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(1, &RenderFBO);
		glGenFramebuffers(1, &TempRenderFBO);

		return true;
	}

	bool begin()
	{
		bool Validated = this->checkExtension("GL_NVX_linked_gpu_multicast");

		caps Caps(caps::CORE);

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if (Validated)
			Validated = initFramebuffer();
		if(Validated)
			Validated = initTexture();
		
		return Validated;
	}

	bool end()
	{
	

		return true;
	}

	bool render()
	{
		// Get FB Size
		glm::ivec2 WindowSize(this->getWindowSize());
		glm::ivec2 const FramebufferSize(WindowSize);

		// bind scene data UBO 
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBOBuffer);

		// prepare an FBO to render into
		glBindFramebuffer(GL_FRAMEBUFFER, RenderFBO);
		
		// Set Viewport
		glViewport(0, 0, FramebufferSize.x / 2.0, FramebufferSize.y);
		glUseProgram(ProgramRender);

		// Setup Multicast - left view
		sceneData.Color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glLGPUNamedBufferSubDataNVX( GPUMASK_0, UBOBuffer, 0, sizeof(SceneData), &sceneData );

		// Setup Multicast - right view
		sceneData.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glLGPUNamedBufferSubDataNVX( GPUMASK_1, UBOBuffer, 0, sizeof(SceneData), &sceneData );

		// use left texture as render target
		glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureLeft, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		// render image
		glBindVertexArray(VertexArrayName);
		glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 1);

		// make sure colorTexRight is safe to write
		glLGPUInterlockNVX();

		// copy the left texture on GPU 1 to the right texture on GPU 0
		glLGPUCopyImageSubDataNVX(
			1,                                        
			GPUMASK_0,                                     
			TextureLeft,  GL_TEXTURE_2D, 0, 0, 0, 0,
			TextureRight, GL_TEXTURE_2D, 0, 0, 0, 0,
			FramebufferSize.x / 2.0, FramebufferSize.y,
			1
		);

		// make sure colorTexRight is complete
		glLGPUInterlockNVX();

		// Compose the Images into backbuffer (complete viewport)
		// render complete viewport to back buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glUseProgram( ProgramCompose );

		// set & upload compose data  
		composeData.out_width  = FramebufferSize.x;
		composeData.out_height = FramebufferSize.y;
		composeData.in_width   = FramebufferSize.x / 2.0;
		composeData.in_height  = FramebufferSize.y;
		glNamedBufferSubDataEXT(ComposeUBO, 0, sizeof(ComposeData), &composeData);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ComposeUBO);

		// use rendered textures as input textures
		glBindMultiTextureEXT( GL_TEXTURE0 + 0, GL_TEXTURE_2D, TextureLeft );
		glBindMultiTextureEXT( GL_TEXTURE0 + 1, GL_TEXTURE_2D, TextureRight );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// render one triangle covering the whole viewport
		glBindVertexArray(VertexArrayName);
		glDrawArrays( GL_TRIANGLES, 0, VertexCount);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	// Put the SLI in multicast mode
	putenv("GL_NVX_LINKED_GPU_MULTICAST=1");

	instance Test(argc, argv);
	Error += Test();

	return Error;
}



