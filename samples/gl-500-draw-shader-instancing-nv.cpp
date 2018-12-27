#include "test.hpp"

#define GL_MESH_SHADER_NV                                      0x9559
#define GL_TASK_SHADER_NV                                      0x955A

#define GL_MAX_MESH_UNIFORM_BLOCKS_NV                          0x8E60
#define GL_MAX_MESH_TEXTURE_IMAGE_UNITS_NV                     0x8E61
#define GL_MAX_MESH_IMAGE_UNIFORMS_NV                          0x8E62
#define GL_MAX_MESH_UNIFORM_COMPONENTS_NV                      0x8E63
#define GL_MAX_MESH_ATOMIC_COUNTER_BUFFERS_NV                  0x8E64
#define GL_MAX_MESH_ATOMIC_COUNTERS_NV                         0x8E65
#define GL_MAX_MESH_SHADER_STORAGE_BLOCKS_NV                   0x8E66
#define GL_MAX_COMBINED_MESH_UNIFORM_COMPONENTS_NV             0x8E67

#define GL_MAX_TASK_UNIFORM_BLOCKS_NV                          0x8E68
#define GL_MAX_TASK_TEXTURE_IMAGE_UNITS_NV                     0x8E69
#define GL_MAX_TASK_IMAGE_UNIFORMS_NV                          0x8E6A
#define GL_MAX_TASK_UNIFORM_COMPONENTS_NV                      0x8E6B
#define GL_MAX_TASK_ATOMIC_COUNTER_BUFFERS_NV                  0x8E6C
#define GL_MAX_TASK_ATOMIC_COUNTERS_NV                         0x8E6D
#define GL_MAX_TASK_SHADER_STORAGE_BLOCKS_NV                   0x8E6E
#define GL_MAX_COMBINED_TASK_UNIFORM_COMPONENTS_NV             0x8E6F

#define GL_MAX_MESH_WORK_GROUP_INVOCATIONS_NV                  0x95A2
#define GL_MAX_TASK_WORK_GROUP_INVOCATIONS_NV                  0x95A3

#define GL_MAX_MESH_TOTAL_MEMORY_SIZE_NV                       0x9536
#define GL_MAX_TASK_TOTAL_MEMORY_SIZE_NV                       0x9537

#define GL_MAX_MESH_OUTPUT_VERTICES_NV                         0x9538
#define GL_MAX_MESH_OUTPUT_PRIMITIVES_NV                       0x9539

#define GL_MAX_TASK_OUTPUT_COUNT_NV                            0x953A

#define GL_MAX_DRAW_MESH_TASKS_COUNT_NV                        0x953D

#define GL_MAX_MESH_VIEWS_NV                                   0x9557

#define GL_MESH_OUTPUT_PER_VERTEX_GRANULARITY_NV               0x92DF
#define GL_MESH_OUTPUT_PER_PRIMITIVE_GRANULARITY_NV            0x9543

#define GL_MAX_MESH_WORK_GROUP_SIZE_NV                         0x953B
#define GL_MAX_TASK_WORK_GROUP_SIZE_NV                         0x953C

#define GL_MESH_WORK_GROUP_SIZE_NV                             0x953E
#define GL_TASK_WORK_GROUP_SIZE_NV                             0x953F

#define GL_MESH_VERTICES_OUT_NV                                0x9579
#define GL_MESH_PRIMITIVES_OUT_NV                              0x957A
#define GL_MESH_OUTPUT_TYPE_NV                                 0x957B

#define GL_UNIFORM_BLOCK_REFERENCED_BY_MESH_SHADER_NV          0x959C
#define GL_UNIFORM_BLOCK_REFERENCED_BY_TASK_SHADER_NV          0x959D

#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_MESH_SHADER_NV  0x959E
#define GL_ATOMIC_COUNTER_BUFFER_REFERENCED_BY_TASK_SHADER_NV  0x959F

#define GL_REFERENCED_BY_MESH_SHADER_NV                        0x95A0
#define GL_REFERENCED_BY_TASK_SHADER_NV                        0x95A1

#define GL_MESH_SUBROUTINE_NV                                  0x957C
#define GL_TASK_SUBROUTINE_NV                                  0x957D

#define GL_MESH_SUBROUTINE_UNIFORM_NV                          0x957E
#define GL_TASK_SUBROUTINE_UNIFORM_NV                          0x957F

#define GL_MESH_SHADER_BIT_NV                                  0x00000040
#define GL_TASK_SHADER_BIT_NV                                  0x00000080

typedef void (GLAPIENTRY * PFNGLDRAWMESHTASKSNVPROC) (GLint first, GLint count);
typedef void (GLAPIENTRY * PFNGLDRAWMESHTASKSINDIRECTNVPROC) (GLintptr indirect);
typedef void (GLAPIENTRY * PFNGLMULTIDRAWMESHTASKSINDIRECTNVPROC) (GLintptr indirect, GLsizei drawcount, GLsizei stride);
typedef void (GLAPIENTRY * PFNGLMULTIDRAWMESHTASKSINDIRECTCOUNTNVPROC) (GLintptr indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);

PFNGLDRAWMESHTASKSNVPROC glDrawMeshTasksNV = 0;
PFNGLDRAWMESHTASKSINDIRECTNVPROC glDrawMeshTasksIndirectNV = 0;
PFNGLMULTIDRAWMESHTASKSINDIRECTNVPROC glMultiDrawMeshTasksIndirectNV = 0;
PFNGLMULTIDRAWMESHTASKSINDIRECTCOUNTNVPROC glMultiDrawMeshTasksIndirectCountNV = 0;

namespace
{
	char const* TASK_SHADER_SOURCE("gl-500/draw-shader-instancing-nv.task");
	char const* MESH_SHADER_SOURCE("gl-500/draw-shader-instancing-nv.mesh");
	char const* FRAG_SHADER_SOURCE("gl-500/draw-shader-instancing-nv.frag");
	char const* TEXTURE_DIFFUSE("kueken7_rgb8_unorm.ktx");

	GLsizei const VertexCount(4);
	GLsizeiptr const VertexSize = VertexCount * sizeof(glf::vertex_v2fv2f);
	glf::vertex_v2fv2f const VertexData[VertexCount] =
	{
		glf::vertex_v2fv2f(glm::vec2(-1.0f,-1.0f), glm::vec2(0.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f,-1.0f), glm::vec2(1.0f, 1.0f)),
		glf::vertex_v2fv2f(glm::vec2( 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
		glf::vertex_v2fv2f(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f))
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLubyte);
	GLubyte const ElementData[ElementCount] =
	{
		0, 1, 2,
		2, 3, 0
	};

	namespace buffer
	{
		enum type
		{
			OBJECT,
			VERTEX,
			ELEMENT,
			TRANSFORM,
			MAX
		};
	}//namespace buffer
	
	namespace shader
	{
		enum type
		{
			VERT,
			FRAG,
			MAX
		};
	}//namespace shader
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-500-draw-shader-instancing-nv", framework::CORE, 4, 6)
		, VertexArrayName(0)
		, ProgramName(0)
		, TextureName(0)
	{}

private:
	std::array<GLuint, buffer::MAX> BufferName;
	GLuint VertexArrayName;
	GLuint ProgramName;
	GLuint TextureName;
	glm::uint8* UniformPointer;

	bool initExtensions()
	{
		if(!this->checkExtension("GL_NV_mesh_shader"))
			return false;

		glDrawMeshTasksNV = (PFNGLDRAWMESHTASKSNVPROC)glfwGetProcAddress("glDrawMeshTasksNV");
		glDrawMeshTasksIndirectNV = (PFNGLDRAWMESHTASKSINDIRECTNVPROC)glfwGetProcAddress("glDrawMeshTasksIndirectNV");
		glMultiDrawMeshTasksIndirectNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTNVPROC)glfwGetProcAddress("glMultiDrawMeshTasksIndirectNV");
		glMultiDrawMeshTasksIndirectCountNV = (PFNGLMULTIDRAWMESHTASKSINDIRECTCOUNTNVPROC)glfwGetProcAddress("glMultiDrawMeshTasksIndirectCountNV");

		return true;
	}

	bool initCaps()
	{
		GLint Query = 0;
		glGetIntegerv(GL_MAX_MESH_WORK_GROUP_INVOCATIONS_NV, &Query);
		printf("GL_MAX_MESH_WORK_GROUP_INVOCATIONS_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_TASK_WORK_GROUP_INVOCATIONS_NV, &Query);
		printf("GL_MAX_TASK_WORK_GROUP_INVOCATIONS_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_MESH_TOTAL_MEMORY_SIZE_NV, &Query);
		printf("GL_MAX_MESH_TOTAL_MEMORY_SIZE_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_TASK_TOTAL_MEMORY_SIZE_NV, &Query);
		printf("GL_MAX_TASK_TOTAL_MEMORY_SIZE_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_MESH_OUTPUT_VERTICES_NV, &Query);
		printf("GL_MAX_MESH_OUTPUT_VERTICES_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_MESH_OUTPUT_PRIMITIVES_NV, &Query);
		printf("GL_MAX_MESH_OUTPUT_PRIMITIVES_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_TASK_OUTPUT_COUNT_NV, &Query);
		printf("GL_MAX_TASK_OUTPUT_COUNT_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_DRAW_MESH_TASKS_COUNT_NV, &Query);
		printf("GL_MAX_DRAW_MESH_TASKS_COUNT_NV: %d\n", Query);

		glGetIntegerv(GL_MAX_MESH_VIEWS_NV, &Query);
		printf("GL_MAX_MESH_VIEWS_NV: %d\n", Query);

		glGetIntegerv(GL_MESH_OUTPUT_PER_VERTEX_GRANULARITY_NV, &Query);
		printf("GL_MESH_OUTPUT_PER_VERTEX_GRANULARITY_NV: %d\n", Query);

		glGetIntegerv(GL_MESH_OUTPUT_PER_PRIMITIVE_GRANULARITY_NV, &Query);
		printf("GL_MESH_OUTPUT_PER_PRIMITIVE_GRANULARITY_NV: %d\n", Query);

		return true;
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint TaskShaderName = Compiler.create(GL_TASK_SHADER_NV, getDataDirectory() + TASK_SHADER_SOURCE, "--version 460 --profile core");
			GLuint MeshShaderName = Compiler.create(GL_MESH_SHADER_NV, getDataDirectory() + MESH_SHADER_SOURCE, "--version 460 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 460 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, TaskShaderName);
			glAttachShader(ProgramName, MeshShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		return Validated;
	}

	bool initBuffer()
	{
		GLsizei const ObjectCount(2);
		GLsizeiptr const ObjectSize = ObjectCount * sizeof(glm::mat4);
		glm::mat4 ObjectData[ObjectCount];

		ObjectData[0] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0)), glm::vec3(0.5f));
		ObjectData[1] = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0)), glm::vec3(0.5f));

		GLint UniformBufferOffset = 0;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &UniformBufferOffset);
		GLint UniformBlockSize = glm::max(GLint(sizeof(glm::mat4)), UniformBufferOffset);

		glCreateBuffers(buffer::MAX, &BufferName[0]);
		glNamedBufferStorage(BufferName[buffer::ELEMENT], ElementSize, ElementData, 0);
		glNamedBufferStorage(BufferName[buffer::VERTEX], VertexSize, VertexData, 0);
		glNamedBufferStorage(BufferName[buffer::OBJECT], ObjectSize, ObjectData, 0);
		glNamedBufferStorage(BufferName[buffer::TRANSFORM], UniformBlockSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

		this->UniformPointer = static_cast<glm::uint8*>(glMapNamedBufferRange(
			BufferName[buffer::TRANSFORM], 0, UniformBlockSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));

		return true;
	}

	bool initTexture()
	{
		gli::texture2d Texture(gli::load((getDataDirectory() + TEXTURE_DIFFUSE).c_str()));

		gli::gl GL(gli::gl::PROFILE_GL32);
		gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glCreateTextures(GL_TEXTURE_2D, 1, &this->TextureName);
		glTextureParameteri(this->TextureName, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(this->TextureName, GL_TEXTURE_MAX_LEVEL, static_cast<GLint>(Texture.levels() - 1));
		glTextureParameteri(this->TextureName, GL_TEXTURE_MIN_FILTER, Texture.levels() == 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(this->TextureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(this->TextureName, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(this->TextureName, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameterf(this->TextureName, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.f);

		glTextureStorage2D(this->TextureName, static_cast<GLint>(Texture.levels()), Format.Internal, static_cast<GLsizei>(Texture.extent().x), static_cast<GLsizei>(Texture.extent().y));
		for(gli::texture2d::size_type Level = 0; Level < Texture.levels(); ++Level)
		{
			gli::texture2d::extent_type const Extent = Texture[Level].extent();
			glTextureSubImage2D(this->TextureName, static_cast<GLint>(Level),
				0, 0,
				static_cast<GLsizei>(Extent.x), static_cast<GLsizei>(Extent.y),
				Format.External, Format.Type,
				Texture[Level].data());
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		return true;
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initExtensions();
		if(Validated)
			Validated = initCaps();
		if(Validated)
			Validated = initBuffer();
		if(Validated)
			Validated = initTexture();
		if(Validated)
			Validated = initProgram();

		return Validated;
	}

	bool end()
	{
		glUnmapNamedBuffer(this->BufferName[buffer::TRANSFORM]);

		glDeleteProgram(this->ProgramName);
		glDeleteBuffers(buffer::MAX, &this->BufferName[0]);
		glDeleteTextures(1, &this->TextureName);
		glDeleteVertexArrays(1, &this->VertexArrayName);

		return true;
	}

	bool render()
	{
		glm::vec2 const WindowSize(this->getWindowSize());

		glm::mat4 const Projection = glm::perspectiveFov(glm::pi<float>() * 0.25f, WindowSize.x, WindowSize.y, 0.1f, 100.0f);
		*reinterpret_cast<glm::mat4*>(this->UniformPointer) = Projection * this->view();

		glDrawBuffer(GL_BACK);
		glDisable(GL_FRAMEBUFFER_SRGB);

		glViewport(0, 0, static_cast<GLsizei>(WindowSize.x), static_cast<GLsizei>(WindowSize.y));
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glUseProgram(this->ProgramName);
		glBindTextureUnit(0, this->TextureName);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, BufferName[buffer::TRANSFORM]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, BufferName[buffer::ELEMENT]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, BufferName[buffer::VERTEX]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, BufferName[buffer::OBJECT]);

		glDrawMeshTasksNV(0, 1);

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
