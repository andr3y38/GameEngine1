#pragma once
#include <GL/glew.h>

namespace GE {
	bool compileProgram(const char* v_shader_sourcecode[], const char* f_shader_sourcecode[], GLuint* programId);
}