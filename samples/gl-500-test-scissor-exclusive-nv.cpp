#include "test.hpp"

#define GL_SCISSOR_TEST_EXCLUSIVE_NV 0x9555

typedef void (GLAPIENTRY * PFNGLSCISSOREXCLUSIVEARRAYVNVPROC) (GLuint first, GLsizei count, const GLint * v);

PFNGLSCISSOREXCLUSIVEARRAYVNVPROC glScissorExclusiveArrayvNV = 0;

namespace
{
	char const* VERT_SHADER_SOURCE("gl-500/test-scissor-exclusive-nv.vert");
	char const* FRAG_SHADER_SOURCE("gl-500/test-scissor-exclusive-nv.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgba8_srgb.dds");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
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
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-500-test-scissor-exclusive-nv", framework::CORE, 4, 6, glm::vec2(glm::pi<float>() * 0.2f))
		, VertexArrayName(0)
		, PipelineName(0)
		, ProgramName(0)
		, TextureName(0)
		, UniformPointer(nullptr)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint VertexArrayName;
	GLuint PipelineName;
	GLuint ProgramName;
	GLuint TextureName;
	glm::uint8* UniformPointer;

	bool initExtensions()
	{
		if(!this->checkExtension("GL_NV_scissor_exclusive"))
			return false;

		glScissorExclusiveArrayvNV = (PFNGLSCISSOREXCLUSIVEARRAYVNVPROC)glfwGetProcAddress("glScissorExclusiveArrayvNV");

		return true;
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 460 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 460 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		if(Validated)
		{
			glCreateProgramPipelines(1, &this->PipelineName);
			glUseProgramStages(this->PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);
		}

		return Validated;
	}

	bool initBuffer()
	{
		GLint UniformBufferOffset(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::ELEMENT], ElementSize, ElementData, 0);
		glNamedBufferStorage(BufferName[buffer::VERTEX], VertexSize, VertexData, 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		this->UniformPointer = static_cast<glm::uint8*>(glMapNamedBufferRange(
			BufferName[buffer::TRANSFORM], 0, UniformBlockSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		return true;
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load_dds((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));

		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glCreateTextures(GL_TEXTURE_2D, 1, &this->TextureName);
		glTextureParameteri(this->TextureName, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(this->TextureName, GL_TEXTURE_MAX_LEVEL, GLint(Texture.levels() - 1));
		glTextureParameteri(this->TextureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(this->TextureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(this->TextureName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(this->TextureName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTextureStorage2D(this->TextureName, static_cast<GLint>(Texture.levels()), Format.Internal, static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y));
		for(std::size_t Level = 0; Level < Texture.levels(); ++Level)
		{
			glTextureSubImage2D(this->TextureName, GLint(Level),
				0, 0,
				GLsizei(Texture[Level].extent().x), GLsizei(Texture[Level].extent().y),
				Format.External, Format.Type,
				Texture[Level].data());
		}

		return true;
	}

	bool initVertexArray()
	{
		glCreateVertexArrays(1, &VertexArrayName);

		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::POSITION, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::POSITION);

		glVertexArrayAttribBinding(VertexArrayName, semantic::attr::TEXCOORD, 0);
		glVertexArrayAttribFormat(VertexArrayName, semantic::attr::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2));
		glEnableVertexArrayAttrib(VertexArrayName, semantic::attr::TEXCOORD);

		glVertexArrayElementBuffer(VertexArrayName, BufferName[buffer::ELEMENT]);
		glVertexArrayVertexBuffer(VertexArrayName, 0, BufferName[buffer::VERTEX], 0, sizeof(glf::vertex_v2fv2f));

		return true;
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initExtensions();
		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initVertexArray();

		return Validated;
	}

	bool end()
	{
		glUnmapNamedBuffer(this->BufferName[buffer::TRANSFORM]);

		glDeleteProgramPipelines(1, &this->PipelineName);
		glDeleteBuffers(buffer::MAX, &BufferName[0]);
		glDeleteProgram(ProgramName);
		glDeleteTextures(1, &TextureName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::ivec2 WindowSize(this->getWindowSize());

		glm::vec3 MinScissor( 10000.f);
		glm::vec3 MaxScissor(-10000.f);

		{
			glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 View = this->view();
			glm::mat4 Model = glm::mat4(1.0f);
		
			*reinterpret_cast<glm::mat4*>(this->UniformPointer) = Projection * View * Model;

			glm::mat4 Ortho = glm::ortho(0.0f, 0.0f, float(WindowSize.x), float(WindowSize.y));
			for(GLsizei i = 0; i < VertexCount; ++i)
			{
				glm::vec3 Projected = glm::project(
					glm::vec3(VertexData[i].Position, 0.0f), 
					View * Model, 
					Projection, 
					glm::ivec4(0, 0, WindowSize.x, WindowSize.y));

				MinScissor = glm::min(MinScissor, glm::vec3(Projected));
				MaxScissor = glm::max(MaxScissor, glm::vec3(Projected));
			}
		}

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		GLint const Scissor[] = {GLint(MinScissor.x), GLint(MinScissor.y), GLsizei(MaxScissor.x - MinScissor.x), GLsizei(MaxScissor.y - MinScissor.y)};
		glScissorExclusiveArrayvNV(0, 1, Scissor);
		glEnable(GL_SCISSOR_TEST_EXCLUSIVE_NV);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

		glBindProgramPipeline(this->PipelineName);
		glBindTextureUnit(0, this->TextureName);
		glBindBufferBase(GL_UNIFORM_BUFFER, semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM]);
		glBindVertexArray(VertexArrayName);

		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, ElementCount, GL_UNSIGNED_SHORT, 0, 1, 0);

		glDisable(GL_SCISSOR_TEST_EXCLUSIVE_NV);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error = 0;

	sample Sample(argc, argv);
	Error += Sample();

	return Error;
}

