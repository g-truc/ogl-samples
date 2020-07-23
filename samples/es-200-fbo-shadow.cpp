#include "test.hpp"

namespace
{
	char const* VERT_SHADER_SOURCE_DEPTH("es-200/fbo-shadow-depth.vert");
	char const* FRAG_SHADER_SOURCE_DEPTH("es-200/fbo-shadow-depth.frag");
	char const* VERT_SHADER_SOURCE_RENDER("es-200/fbo-shadow-render.vert");
	char const* FRAG_SHADER_SOURCE_RENDER("es-200/fbo-shadow-render.frag");

	GLsizei const VertexCount(8);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v3fv4u8);
	glf::vertex_v3fv4u8 const VertexData[VertexCount] =
	{
		glf::vertex_v3fv4u8(glm::vec3(-1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 1.0f,-1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-1.0f, 1.0f, 0.0f), glm::u8vec4(255, 127,   0, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 0.1f,-0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3( 0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255)),
		glf::vertex_v3fv4u8(glm::vec3(-0.1f, 0.1f, 0.2f), glm::u8vec4(  0, 127, 255, 255))
	};

	GLsizei const ElementCount(12);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0,
		4, 5, 6, 
		6, 7, 4,
	};

	namespace buffer
	{
		enum type
		{
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer

	namespace attachment
	{
		enum type
		{
			COLORBUFFER,
			DEPTHBUFFER,
			MAX
		};
	}//namespace attachment
	
	namespace framebuffer
	{
		enum type
		{
			RENDER,
			DEPTH,
			MAX
		};
	}//namespace framebuffer

	namespace shader
	{
		enum type
		{
			VERT_RENDER,
			FRAG_RENDER,
			VERT_DEPTH,
			FRAG_DEPTH,
			MAX
		};
	}//namespace shader

	glm::ivec2 const ShadowSize(64, 64);
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[]) :
		framework(argc, argv, "es-200-fbo-shadow", framework::ES, 2, 0, glm::vec2(0.0f, -glm::pi<float>() * 0.3f))
	{}

private:
	std::array<GLuint, framebuffer::MAX> FramebufferName;
	std::array<GLuint, framebuffer::MAX> ProgramName;
	std::array<GLuint, buffer::MAX> BufferName;
	std::array<GLuint, attachment::MAX> TextureName;
	std::array<GLuint, attachment::MAX> RenderbufferName;
	GLint UniformLightProj;
	GLint UniformLightView;
	GLint UniformLightWorld;
	GLint UniformLightPointLightPosition;
	GLint UniformLightClipNearFar;

	GLint UniformRenderP;
	GLint UniformRenderV;
	GLint UniformRenderW;
	GLint UniformRenderShadow;
	GLint UniformRenderPointLightPosition;
	GLint UniformRenderClipNearFar;
	GLint UniformRenderBias;

	bool initProgram()
	{
		bool Validated(true);

		std::vector<GLuint> ShaderName(shader::MAX);

		if(Validated)
		{
			compiler Compiler;
			ShaderName[shader::VERT_DEPTH] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_DEPTH);
			ShaderName[shader::FRAG_DEPTH] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_DEPTH);
			Validated = Validated && Compiler.check();

			ProgramName[framebuffer::DEPTH] = glCreateProgram();
			glAttachShader(ProgramName[framebuffer::DEPTH], ShaderName[shader::VERT_DEPTH]);
			glAttachShader(ProgramName[framebuffer::DEPTH], ShaderName[shader::FRAG_DEPTH]);
			glBindAttribLocation(ProgramName[framebuffer::DEPTH], semantic::attr::POSITION, "Position");
			glLinkProgram(ProgramName[framebuffer::DEPTH]);

			Validated = Validated && Compiler.check_program(ProgramName[framebuffer::DEPTH]);
		}

		if(Validated)
		{
			this->UniformLightProj = glGetUniformLocation(ProgramName[framebuffer::DEPTH], "LightProj");
			this->UniformLightView = glGetUniformLocation(ProgramName[framebuffer::DEPTH], "LightView");
			this->UniformLightWorld = glGetUniformLocation(ProgramName[framebuffer::DEPTH], "LightWorld");
			this->UniformLightPointLightPosition = glGetUniformLocation(ProgramName[framebuffer::DEPTH], "PointLightPosition");
			this->UniformLightClipNearFar = glGetUniformLocation(ProgramName[framebuffer::DEPTH], "ShadowClipNearFar");
		}

		if(Validated)
		{
			compiler Compiler;
			ShaderName[shader::VERT_RENDER] = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE_RENDER);
			ShaderName[shader::FRAG_RENDER] = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE_RENDER);
			Validated = Validated && Compiler.check();

			ProgramName[framebuffer::RENDER] = glCreateProgram();
			glAttachShader(ProgramName[framebuffer::RENDER], ShaderName[shader::VERT_RENDER]);
			glAttachShader(ProgramName[framebuffer::RENDER], ShaderName[shader::FRAG_RENDER]);
			glBindAttribLocation(ProgramName[framebuffer::RENDER], semantic::attr::POSITION, "Position");
			glBindAttribLocation(ProgramName[framebuffer::RENDER], semantic::attr::COLOR, "Color");
			glLinkProgram(ProgramName[framebuffer::RENDER]);

			Validated = Validated && Compiler.check_program(ProgramName[framebuffer::RENDER]);
		}

		if(Validated)
		{
			UniformRenderP = glGetUniformLocation(ProgramName[framebuffer::RENDER], "P");
			UniformRenderV = glGetUniformLocation(ProgramName[framebuffer::RENDER], "V");
			UniformRenderW = glGetUniformLocation(ProgramName[framebuffer::RENDER], "W");
			UniformRenderShadow = glGetUniformLocation(ProgramName[framebuffer::RENDER], "Shadow");
			UniformRenderPointLightPosition = glGetUniformLocation(ProgramName[framebuffer::RENDER], "PointLightPosition");
			UniformRenderClipNearFar = glGetUniformLocation(ProgramName[framebuffer::RENDER], "ShadowClipNearFar");
			UniformRenderBias = glGetUniformLocation(ProgramName[framebuffer::RENDER], "Bias");
		}

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
		glm::ivec2 const WindowSize(this->getWindowSize());

		glGenRenderbuffers(attachment::MAX, &this->RenderbufferName[0]);

		glBindRenderbuffer(GL_RENDERBUFFER, this->RenderbufferName[attachment::COLORBUFFER]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, GLsizei(WindowSize.x), GLsizei(WindowSize.y));

		glBindRenderbuffer(GL_RENDERBUFFER, this->RenderbufferName[attachment::DEPTHBUFFER]);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, GLsizei(ShadowSize.x), GLsizei(ShadowSize.y));

		glGenTextures(attachment::MAX, &this->TextureName[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TextureName[attachment::COLORBUFFER]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GLsizei(WindowSize.x), GLsizei(WindowSize.y), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TextureName[attachment::DEPTHBUFFER]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ShadowSize.x, ShadowSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		return true;
	}

	bool initFramebuffer()
	{
		glGenFramebuffers(framebuffer::MAX, &FramebufferName[0]);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::DEPTH]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->TextureName[framebuffer::DEPTH], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RenderbufferName[framebuffer::DEPTH]);
		if(!this->checkFramebuffer(FramebufferName[framebuffer::DEPTH]))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::RENDER]);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->TextureName[framebuffer::RENDER], 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->RenderbufferName[framebuffer::RENDER]);
		if(!this->checkFramebuffer(FramebufferName[framebuffer::RENDER]))
			return false;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		return true;
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initFramebuffer();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glVertexAttribPointer(semantic::attr::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glf::vertex_v3fv4u8), BUFFER_OFFSET(0));
		glVertexAttribPointer(semantic::attr::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(glf::vertex_v3fv4u8), BUFFER_OFFSET(sizeof(glm::vec3)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(semantic::attr::POSITION);
		glEnableVertexAttribArray(semantic::attr::COLOR);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		for(std::size_t i = 0; i < framebuffer::MAX; ++i)
			glDeleteProgram(ProgramName[i]);

		glDeleteFramebuffers(framebuffer::MAX, &FramebufferName[0]);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteTextures(framebuffer::MAX, &TextureName[0]);

		return this->checkError("end");
	}

	void renderShadow()
	{
		glViewport(0, 0, ShadowSize.x, ShadowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName[framebuffer::DEPTH]);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);

		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr);

		this->checkError("renderShadow");
	}

	void renderFramebuffer()
	{
		glm::ivec2 WindowSize(this->getWindowSize());
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		float Depth(1.0f);
		glClearBufferfv(GL_DEPTH , 0, &Depth);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->TextureName[framebuffer::DEPTH]);

		glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, nullptr);

		this->checkError("renderFramebuffer");
	}

	bool render()
	{
		glm::ivec2 const WindowSize(this->getWindowSize());

		{
			glm::mat4 const LightP = glm::perspective(glm::pi<float>() * 0.25f, 1.0f, 0.1f, 10.0f);
			glm::mat4 const LightV = glm::lookAt(glm::vec3(0.5, 1.0, 2.0), glm::vec3(0), glm::vec3(0, 0, 1));
			glm::mat4 const LightW(1.0f);

			glUseProgram(ProgramName[framebuffer::DEPTH]);
			glUniformMatrix4fv(this->UniformLightProj, 1, GL_FALSE, &LightP[0][0]);
			glUniformMatrix4fv(this->UniformLightView, 1, GL_FALSE, &LightV[0][0]);
			glUniformMatrix4fv(this->UniformLightWorld, 1, GL_FALSE, &LightW[0][0]);
			glUniform3f(this->UniformLightPointLightPosition, 0.f, 0.f, 10.f);
			glUniform2f(this->UniformLightClipNearFar, 0.01f, 10.0f);

			renderShadow();
		}

		{
			glm::mat4 const RenderP = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 10.0f);
			glm::mat4 const RenderV = this->view();
			glm::mat4 const RenderW(1.0f);

			glUseProgram(ProgramName[framebuffer::RENDER]);
			glUniformMatrix4fv(this->UniformRenderP, 1, GL_FALSE, &RenderP[0][0]);
			glUniformMatrix4fv(this->UniformRenderV, 1, GL_FALSE, &RenderV[0][0]);
			glUniformMatrix4fv(this->UniformRenderW, 1, GL_FALSE, &RenderW[0][0]);
			glUniform1i(this->UniformRenderShadow, 0);
			glUniform3f(this->UniformRenderPointLightPosition, 0.f, 0.f, 10.f);
			glUniform2f(this->UniformRenderClipNearFar, 0.01f, 10.0f);
			glUniform1f(this->UniformRenderBias, 0.002f);

			renderFramebuffer();
		}

		return this->checkError("render");
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	sample Sample(argc, argv);
	Error += Sample();

	return Error;
}

