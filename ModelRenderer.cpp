#include "ModelRenderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderUtils.h"

namespace GE {
	
	ModelRenderer::ModelRenderer(Model* m) {
	
		pos_x = pos_y = pos_z = 5.0f;
		rot_x = rot_y = rot_z = 0.0f;
		scale_x = 5.0f;
		scale_y = 5.0f;
		scale_z = 10.0f;

		model = m;
	}

	ModelRenderer::~ModelRenderer()
	{
	}

	//This is a helper function that allows us to see
	//shader compiler error messages should our shaders not compile okay
	void displayShaderCompilerError(GLuint shaderId) {
		//First, get the lenght of the error message string 
		GLint MsgLen = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

		//Only display the message is more than 1 character was returned 
		if (MsgLen > 1) {
			//Create a character buffer to store the error message characters 
			GLchar* Msg = new GLchar[MsgLen + 1];

			//Get the error message characters from the OpenGL internal log 
			//into the Msg buffer
			glGetShaderInfoLog(shaderId, MsgLen, NULL, Msg);

			//Display the error message so we can see what the problem
			//was with the shader
			std::cerr << "Error compiling shader" << Msg << std::endl;

			//Release the memory allocated to the string
			delete[] Msg;
		}
	}

	
	void ModelRenderer::init() {
		
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		//The shader code as described in lecture
		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec3 vertexPos3D;\n"
			"in vec2 vUV;\n"
			"out vec2 uv;\n"
			"uniform mat4 transform;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projection * view * transform * v;\n"
			"gl_Position = v;\n"
			"uv = vUV;\n"
			"}\n" };

		//Copy the source to OpenGL ready for compilation
		glShaderSource(vertexShader, 1, V_ShaderCode, NULL);

		//Compile the code
		glCompileShader(vertexShader);

		//Check for compiler errors
		//Presume shader didn't compile
		GLint isShaderCompiledOK = GL_FALSE;

		//Get the compile status from OpenGL
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		//Has the shader failed to compile?
		if (isShaderCompiledOK != GL_TRUE) {
			//Yes, so display an error message
			std::cerr << "Unable to compile vertex shader" << std::endl;

			displayShaderCompilerError(vertexShader);

			return;
		}
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		//Fragment shader source code
		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"in vec2 uv;\n"
			"uniform sampler2D sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = texture(sampler,uv).rgba;\n"
			"}\n" };

		//Transfer the shader code
		glShaderSource(fragmentShader, 1, F_ShaderCode, NULL);

		//Compile the code
		glCompileShader(fragmentShader);

		//Check for compiler errors
		isShaderCompiledOK = GL_FALSE;

		//Get the compile status from OpenGL
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		//Has the shader failed to compile?
		if (isShaderCompiledOK != GL_TRUE) {
			//Yes, so display an error message
			std::cerr << "Unable to compile fragment shader" << std::endl;

			displayShaderCompilerError(fragmentShader);

			return;
		}

		//Create the program object
		programId = glCreateProgram();

		//Attach shaders to the program object
		glAttachShader(programId, vertexShader);
		glAttachShader(programId, fragmentShader);

		//Link the program to create an executable program we 
		//use to render the object
		//Program executable will exist in graphics memory
		glLinkProgram(programId);

		//Check for linking errors
		GLint isProgramLinked = GL_FALSE;
		glGetProgramiv(programId, GL_LINK_STATUS, &isProgramLinked);
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program" << std::endl;
		}

		//Now get a link to the vertexPos2D so we can link the attribute 
		//to our vertices when rendering 
		vertexPos3DLocation = glGetAttribLocation(programId, "vertexPos3D");

		//check for errors
		if (vertexPos3DLocation == -1) {
			std::cerr << "Problem gettting vertex3DPos" << std::endl;
		}

		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		//Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		//Link the uniforms to the member fields
		transformUniformId = glGetUniformLocation(programId, "transform");
		viewUniformId = glGetUniformLocation(programId, "view");
		projectionUniformId = glGetUniformLocation(programId, "projection");
		samplerId = glGetUniformLocation(programId, "sampler");

		//Create the vertex buffer object
		glGenBuffers(1, &vboModel);
		glBindBuffer(GL_ARRAY_BUFFER, vboModel);

		//Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, model->getNumVertices() * sizeof(Vertex), model -> getVertices(), GL_STATIC_DRAW);
	}

	void ModelRenderer::update()
	{
	}

	void ModelRenderer::draw(Camera* cam) {

		glEnable(GL_CULL_FACE);

		//Calculate the transformation matrix for the object. Start with the identity matrix
		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		//Get the view and projection matrices
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		//Select the program into the rendering context
		glUseProgram(programId);

		//Set the uniforms in the shader
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		
		glEnableVertexAttribArray(vertexPos3DLocation);
		//Define the structure of a vertex for OpenGL to select values from vertex buffer
		//and store in vertexPos2DLocation attribute
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		glEnableVertexAttribArray(vertexUVLocation);

		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		

		//Enable the attribute to be passed vertices from the vertex buffer object
		

		//Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboModel);
		
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, material->getTextureName());

		//Draw the model
		glDrawArrays(GL_TRIANGLES, 0, model -> getNumVertices());

		//Unselect the attribute from the context
		glDisableVertexAttribArray(vertexPos3DLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		//Unselect the program from the context
		glUseProgram(0);


	}

	//Release objects allocated for program and vertex buffer object
	void ModelRenderer::destroy() {
		glDeleteProgram(programId);

		glDeleteBuffers(1, &vboModel);
	}




}
