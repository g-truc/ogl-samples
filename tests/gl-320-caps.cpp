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

class gl_320_caps : public test
{
public:
	gl_320_caps(int argc, char* argv[]) :
		test(argc, argv, "gl-320-caps", test::CORE, 3, 2)
	{}

private:

	bool begin()
	{
		caps Caps(caps::CORE);

		bool Validated = true;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS >= 12;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS >= 12;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS >= 12;

		Validated = Validated && Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS >= 1024;
		Validated = Validated && Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS >= 1024;

		Validated = Validated && Caps.Limits.MAX_COMBINED_UNIFORM_BLOCKS >= 36;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BUFFER_BINDINGS >= 36;
		Validated = Validated && Caps.Limits.MAX_UNIFORM_BLOCK_SIZE >= 16384;

		Validated = Validated && Caps.Limits.MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS >= ceil(((GLdouble)Caps.Limits.MAX_VERTEX_UNIFORM_BLOCKS / 4.0f * (GLdouble)Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) + Caps.Limits.MAX_VERTEX_UNIFORM_COMPONENTS);
		Validated = Validated && Caps.Limits.MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS >= ceil(((GLdouble)Caps.Limits.MAX_GEOMETRY_UNIFORM_BLOCKS / 4.0f * (GLdouble)Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) + Caps.Limits.MAX_GEOMETRY_UNIFORM_COMPONENTS);
		Validated = Validated && Caps.Limits.MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS >= ceil(((GLdouble)Caps.Limits.MAX_FRAGMENT_UNIFORM_BLOCKS / 4.0f * (GLdouble)Caps.Limits.MAX_UNIFORM_BLOCK_SIZE) + Caps.Limits.MAX_FRAGMENT_UNIFORM_COMPONENTS);

		return Validated;
	}

	bool end()
	{
		return true;
	}

	bool render()
	{
		glm::uvec2 WindowSize = this->getWindowSize();

		glViewport(0, 0, WindowSize.x, WindowSize.y);
		glClearBufferfv(GL_COLOR, 0, &glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)[0]);

		return true;
	}
};

int main(int argc, char* argv[])
{
	int Error(0);

	gl_320_caps Test(argc, argv);
	Error += Test();

	return Error;
}
