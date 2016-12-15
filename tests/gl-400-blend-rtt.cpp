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

namespace
{
	char const * VERTEX_SHADER_SOURCE1("gl-400/blend-rtt.vert");
	char const * FRAGMENT_SHADER_SOURCE1("gl-400/blend-rtt.frag");
	char const * VERTEX_SHADER_SOURCE2("gl-400/blend-rtt-blit.vert");
	char const * FRAGMENT_SHADER_SOURCE2("gl-400/blend-rtt-blit.frag");

	GLsizei const VertexCount = 4;
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 1.0f))
	};

	GLsizei const ElementCount = 6;
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

	namespace texture
	{
		enum type
		{
			R,
			G,
			B,
			MAX
		};
	}//namespace texture

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			MAX
		};
	}//namespace buffer

	namespace program
	{
		enum type
		{
			COLORBUFFERS,
			BLIT,
			MAX
		};
	}//namespace program


	GLint UniformMVPSingle = 0;
	GLint UniformDiffuseSingle = 0;

	GLint UniformMVPMultiple = 0;
}//namespace

class instance : public test
{
public:
	instance(int argc, char* argv[])
		: test(argc, argv, "gl-400-blend-rtt", test::CORE, 4, 0)
		, FramebufferName(0)
		, VertexArrayName(0)
	{}

private:
	GLuint FramebufferName;
	GLuint VertexArrayName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, texture::MAX> TextureName;
	std::array<glm::ivec4, texture::MAX> Viewport;
	std::array<GLuint, program::MAX> ProgramName;

	bool initProgram()
	{
		bool Validated = true;

		compiler Compiler;

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERTEX_SHADER_SOURCE1, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE1, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			this->ProgramName[program::COLORBUFFERS] = glCreateProgram();
			glAttachShader(this->ProgramName[program::COLORBUFFERS], VertShaderName);
			glAttachShader(this->ProgramName[program::COLORBUFFERS], FragShaderName);
			glLinkProgram(this->ProgramName[program::COLORBUFFERS]);
		}

		if(Validated)
		{
			UniformMVPMultiple = glGetUniformLocation(this->ProgramName[program::COLORBUFFERS], "MVP");
		}

		if(Validated)
		{
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERTEX_SHADER_SOURCE2, "--version 400 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAGMENT_SHADER_SOURCE2, "--version 400 --profile core");
			Validated = Validated && Compiler.check();

			this->ProgramName[program::BLIT] = glCreateProgram();
			glAttachShader(this->ProgramName[program::BLIT], VertShaderName);
			glAttachShader(this->ProgramName[program::BLIT], FragShaderName);
			glLinkProgram(this->ProgramName[program::BLIT]);
		}

		if(Validated)
		{
			UniformMVPSingle = glGetUniformLocation(this->ProgramName[program::BLIT], "MVP");
			UniformDiffuseSingle = glGetUniformLocation(this->ProgramName[program::BLIT], "Diffuse");
		}

		Validated = Validated && Compiler.check_program(this->ProgramName[program::COLORBUFFERS]);
		Validated = Validated && Compiler.check_program(this->ProgramName[program::BLIT]);

		return Validated;
	}

	bool initBuffer()
	{
		glGenBuffers(buffer::MAX, &BufferName[0]);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glBufferData(GL_ARRAY_BUFFER, VertexSize, VertexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return true;
	}

	bool initTexture()
	{
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(texture::MAX, &TextureName[0]);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::R]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::G]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		glBindTexture(GL_TEXTURE_2D, TextureName[texture::B]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ZERO);

		for(int i = texture::R; i <= texture::B; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, TextureName[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_GREEN);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_BLUE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_ALPHA);

			glTexImage2D(GL_TEXTURE_2D, 0,
				GL_RGBA8,
				GLsizei(320), GLsizei(240),
				0,
				GL_RGBA, GL_UNSIGNED_BYTE,
				nullptr);
		}

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(1, &FramebufferName);
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName[0], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, TextureName[1], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, TextureName[2], 0);

		GLenum DrawBuffers[4];
		DrawBuffers[0] = GL_NONE;
		DrawBuffers[1] = GL_COLOR_ATTACHMENT0;
		DrawBuffers[2] = GL_COLOR_ATTACHMENT1;
		DrawBuffers[3] = GL_COLOR_ATTACHMENT2;

		glDrawBuffers(4, DrawBuffers);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(0));
			glVertexAttribPointer(semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v2fv2f), BUFFER_OFFSET(sizeof(glm::vec2)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
			glEnableVertexAttribArray(semantic::attr::TEXCOORD);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
		glBindVertexArray(0);

		return true;
	}

	bool initBlend()
	{
		glEnable(GL_SAMPLE_MASK);
		glSampleMaski(0, 0xFF);

		glEnablei(GL_BLEND, 0);
		glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glBlendEquationSeparatei(0, GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparatei(0, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnablei(GL_BLEND, 1);
		glColorMaski(1, GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
		glBlendEquationSeparatei(1, GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparatei(1, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnablei(GL_BLEND, 2);
		glColorMaski(2, GL_FALSE, GL_TRUE, GL_FALSE, GL_TRUE);
		glBlendEquationSeparatei(2, GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparatei(2, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnablei(GL_BLEND, 3);
		glColorMaski(3, GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);
		glBlendEquationSeparatei(3, GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparatei(3, GL_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return true;
	}

	bool begin()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		this->Viewport[texture::R] = glm::ivec4(WindowSize.x >> 1, 0, WindowSize >> 1);
		this->Viewport[texture::G] = glm::ivec4(WindowSize.x >> 1, WindowSize.y >> 1, WindowSize >> 1);
		this->Viewport[texture::B] = glm::ivec4(0, WindowSize.y >> 1, WindowSize >> 1);

		bool Validated = true;

		if(Validated)
			Validated = initBlend();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		return Validated;
	}

	bool end()
	{
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(texture::MAX, &TextureName[0]);
		glDeleteProgram(ProgramName[program::BLIT]);
		glDeleteProgram(ProgramName[program::COLORBUFFERS]);
		glDeleteFramebuffers(1, &FramebufferName);

		return this->checkError("end");
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		// Pass 1: Compute the MVP (Model View Projection matrix)
		glm::mat4 Projection = glm::ortho(-1.0f, 1.0f,-1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 View = ViewTranslate;
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * View * Model;
		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.5f)[0]);
		glClearBufferfv(GL_COLOR, 1, &glm::vec4(0.5f)[1]);
		glClearBufferfv(GL_COLOR, 2, &glm::vec4(0.5f)[2]);
		glClearBufferfv(GL_COLOR, 3, &glm::vec4(0.5f)[3]);

		glUseProgram(ProgramName[program::COLORBUFFERS]);
		glUniformMatrix4fv(UniformMVPMultiple, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(VertexArrayName);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0);

		// Pass 2
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.5f)[0]);

		glUseProgram(ProgramName[program::BLIT]);
		glUniform1i(UniformDiffuseSingle, 0);

		{
			glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, 1.0f,-1.0f, -1.0f, 1.0f);
			glm::mat4 View = glm::mat4(1.0f);
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;
			glUniformMatrix4fv(UniformMVPSingle, 1, GL_FALSE, &MVP[0][0]);
		}

		for(std::size_t i = 0; i < texture::MAX; ++i)
		{
			glViewport(Viewport[i].x, Viewport[i].y, Viewport[i].z, Viewport[i].w);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, TextureName[i]);

			glBindVertexArray(VertexArrayName);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr, 1, 0);
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	instance Test(argc, argv);
	Error += Test();

	return Error;
}
