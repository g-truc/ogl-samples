#include "test.hpp"
#include <chrono>
#include <ratio>

namespace
{
	char const* VERT_SHADER_SOURCE("gl-330/flat-color.vert");
	char const* FRAG_SHADER_SOURCE("gl-330/flat-color.frag");

	GLsizei const VertexCount(6);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f),
		glm::vec2(-1.0f,-1.0f)
	};

	enum synchronization
	{
		SYNC_EXEC,
		SYNC_RENDER,
		SYNC_COUNT
	};

	enum timer
	{
		TIMER_RENDER,
		TIMER_CLEAR,
		TIMER_DRAW,
		TIMER_COUNT
	};

	char const* queryTitle[]
	{
		"RENDER",
		"CLEAR",
		"DRAW"
	};
}//namespace

class sample : public framework
{
public:
	sample(int argc, char* argv[])
		: framework(argc, argv, "gl-330-query-timer", framework::CORE, 3, 3)
		, VertexArrayName(0)
		, ProgramName(0)
		, BufferName(0)
		, UniformMVP(0)
		, UniformColor(0)
	{}

private:
	GLuint VertexArrayName;
	GLuint ProgramName;
	GLuint BufferName;
	std::array<double, SYNC_COUNT> SyncGPUTime;
	std::array<std::chrono::high_resolution_clock::time_point, SYNC_COUNT> SyncCPUTime;
	std::array<GLuint, TIMER_COUNT> QueryName;
	std::array<double, TIMER_COUNT> QueryGPUTime;
	std::array<double, TIMER_COUNT> QueryCPUTime;
	std::array<std::chrono::high_resolution_clock::time_point, TIMER_COUNT> QueryCPUTimeBegin;
	std::array<std::chrono::high_resolution_clock::time_point, TIMER_COUNT> QueryCPUTimeEnd;
	GLint UniformMVP;
	GLint UniformColor;

	bool initQuery()
	{
		glGenQueries(TIMER_COUNT, &QueryName[0]);

		int QueryBits = 0;
		glGetQueryiv(GL_TIME_ELAPSED, GL_QUERY_COUNTER_BITS, &QueryBits);

		bool Validated = QueryBits >= 30;

		return Validated && this->checkError("initQuery");
	}

	bool initProgram()
	{
		bool Validated = true;
	
		if(Validated)
		{
			compiler Compiler;
			GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, getDataDirectory() + VERT_SHADER_SOURCE, "--version 330 --profile core");
			GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, getDataDirectory() + FRAG_SHADER_SOURCE, "--version 330 --profile core");
			Validated = Validated && Compiler.check();

			ProgramName = glCreateProgram();
			glAttachShader(ProgramName, VertShaderName);
			glAttachShader(ProgramName, FragShaderName);
			glLinkProgram(ProgramName);
			Validated = Validated && Compiler.check_program(ProgramName);
		}

		// Get variables locations
		if(Validated)
		{
			UniformMVP = glGetUniformLocation(ProgramName, "MVP");
			UniformColor = glGetUniformLocation(ProgramName, "Diffuse");
		}

		return Validated && this->checkError("initProgram");
	}

	// Buffer update using glBufferSubData
	bool initArrayBuffer()
	{
		// Generate a buffer object
		glGenBuffers(1, &BufferName);

		// Bind the buffer for use
		glBindBuffer(GL_ARRAY_BUFFER, BufferName);

		// Reserve buffer memory but and copy the values
		glBufferData(GL_ARRAY_BUFFER, PositionSize, &PositionData[0][0], GL_STATIC_DRAW);

		// Unbind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return this->checkError("initArrayBuffer");
	}

	bool initVertexArray()
	{
		glGenVertexArrays(1, &VertexArrayName);
		glBindVertexArray(VertexArrayName);
			glBindBuffer(GL_ARRAY_BUFFER, BufferName);
			glVertexAttribPointer(semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(semantic::attr::POSITION);
		glBindVertexArray(0);

		return this->checkError("initVertexArray");
	}

	void syncTimeQuery(synchronization Sync)
	{
		GLint64 TimeGPU = 0;
		glGetInteger64v(GL_TIMESTAMP, &TimeGPU);
		this->SyncGPUTime[Sync] = static_cast<double>(TimeGPU) / 1000.0 / 1000.0;
		this->SyncCPUTime[Sync] = std::chrono::high_resolution_clock::now();
	}

	void beginTimeQuery(timer Timer)
	{
		glBeginQuery(GL_TIME_ELAPSED, this->QueryName[Timer]);
		this->QueryCPUTimeBegin[Timer] = std::chrono::high_resolution_clock::now();
	}

	void endTimeQuery(timer Timer)
	{
		glEndQuery(GL_TIME_ELAPSED);
		this->QueryCPUTimeEnd[Timer] = std::chrono::high_resolution_clock::now();
	}

	void queryTimeQuery(timer Timer)
	{
		GLint64 TimeGPU = 0;
		glGetQueryObjecti64v(QueryName[Timer], GL_QUERY_RESULT, &TimeGPU);
		this->QueryGPUTime[Timer] = static_cast<double>(TimeGPU) / 1000.0 / 1000.0;
		std::chrono::duration<double, std::milli> const Duration = this->QueryCPUTimeEnd[Timer] - this->QueryCPUTimeBegin[Timer];
		this->QueryCPUTime[Timer] = Duration.count();
	}

	void printTimeQuery(timer Timer)
	{
		std::chrono::duration<double, std::milli> const Duration = this->SyncCPUTime[SYNC_RENDER] - this->SyncCPUTime[SYNC_EXEC];

		fprintf(stdout,
			"%s, times: GPU %2.3f ms-i - %2.3f ms-d, CPU %2.3f ms-i - %2.3f ms-d        \r",
			::queryTitle[Timer],
			this->QueryGPUTime[Timer], (this->SyncGPUTime[SYNC_RENDER] - this->SyncGPUTime[SYNC_EXEC] + this->QueryGPUTime[Timer]),
			this->QueryCPUTime[Timer], Duration.count() + this->QueryCPUTime[Timer]);
	}

	bool begin()
	{
		bool Validated = true;

		if(Validated)
			Validated = initProgram();
		if(Validated)
			Validated = initArrayBuffer();
		if(Validated)
			Validated = initVertexArray();
		if(Validated)
			Validated = initQuery();

		syncTimeQuery(SYNC_EXEC);

		return Validated && this->checkError("begin");
	}

	bool end()
	{
		glDeleteBuffers(1, &BufferName);
		glDeleteProgram(ProgramName);
		glDeleteVertexArrays(1, &VertexArrayName);

		return this->checkError("end");
	}

	bool render()
	{
		syncTimeQuery(SYNC_RENDER);
		beginTimeQuery(TIMER_RENDER);

		glm::ivec2 WindowSize(this->getWindowSize());

		glm::mat4 Projection = glm::perspective(glm::pi<float>() * 0.25f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = Projection * this->view() * Model;

		// Set the display viewport
		glViewport(0, 0, WindowSize.x, WindowSize.y);

		beginTimeQuery(TIMER_CLEAR);
			glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);
		endTimeQuery(TIMER_CLEAR);

		// Bind program
		glUseProgram(ProgramName);
		glUniformMatrix4fv(UniformMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniform4fv(UniformColor, 1, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		glBindVertexArray(VertexArrayName);

		// Beginning of the time query
		beginTimeQuery(TIMER_DRAW);
		for(int i = 0; i < 1; ++i)
			glDrawArrays(GL_TRIANGLES, 0, VertexCount);
			//glDrawArraysInstanced(GL_TRIANGLES, 0, VertexCount, 100000);
		endTimeQuery(TIMER_DRAW);

		// Unbind program
		glUseProgram(0);

		// End of the time query
		endTimeQuery(TIMER_RENDER);

		queryTimeQuery(TIMER_RENDER);
		printTimeQuery(TIMER_RENDER);

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
