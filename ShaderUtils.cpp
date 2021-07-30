#include "ShaderUtils.h"
#include <iostream>

namespace GE {
	// This is a helper function that allows us to see
	// shader compiler error messages should our shaders not compile okay
	void _displayShaderCompilerError(GLuint shaderId) {
		// First, get the length of the error message string
		GLint MsgLen = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

		// Only display the message is more than 1 character was returned
		if (MsgLen > 1) {
			// Create a character buffer to store the error message characters
			GLchar* Msg = new GLchar[MsgLen + 1];

			// Get the error message characters from the OpenGL internal log
			// into the Msg buffer
			glGetShaderInfoLog(shaderId, MsgLen, nullptr, Msg);

			// Display the error message so we can see what the problem
			// was with the shader
			std::cerr << "Error compiling shader" << Msg << std::endl;

			// Release the memory allocated to the string
			delete[] Msg;
		}
	}

	bool compileProgram(const GLchar* v_shader_sourcecode[], const GLchar* f_shader_sourcecode[], GLuint* programId) {
		// Create the vertex shader first.
		// Order doesn't matter but shaders must be created and compiled before
		// attaching to program
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Copy the source to OpenGL ready for compilation
		glShaderSource(vertexShader, 1, v_shader_sourcecode, nullptr);

		// Compile the code
		glCompileShader(vertexShader);

		// Check for compiler errors
		// Presume shader didn't compile
		GLint isShaderCompiledOK = GL_FALSE;

		// Get the compile status from OpenGL
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		// Has the shader failed to compile?
		if (isShaderCompiledOK != GL_TRUE) {
			// Yes, so display an error message
			std::cerr << "Unable to compile vertex shader" << std::endl;

			_displayShaderCompilerError(vertexShader);

			return false;
		}

		// Do the same for the fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Transfer the shader code
		glShaderSource(fragmentShader, 1, f_shader_sourcecode, NULL);

		// Compile it
		glCompileShader(fragmentShader);

		// Check for errors.  Code is same as above of getting status
		// and displaying error message, if necessary
		isShaderCompiledOK = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile fragment shader" << std::endl;

			_displayShaderCompilerError(fragmentShader);

			return false;
		}

		// Create the program object
		*programId = glCreateProgram();

		// Attach shaders to the program object
		glAttachShader(*programId, vertexShader);
		glAttachShader(*programId, fragmentShader);

		// Now link the program to create an executable program we
		// and use to render the object
		// Program executable will exist in graphics memory
		glLinkProgram(*programId);

		// Check for linking errors
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(*programId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program" << std::endl;

			return false;
		}

		// Got this far so must be okay, return true
		return true;
	}

}