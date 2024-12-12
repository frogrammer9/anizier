#include "shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>


static uint32_t compileShader(cstr src, GLenum shaderType) {	
	const uint32_t id = glCreateShader(shaderType);
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);
	//error handling
	int shaderCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &shaderCompiled);
	if (shaderCompiled != GL_TRUE) {
		int logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		char* msg = malloc(logLength);
		glGetShaderInfoLog(id, logLength, &logLength, msg);
		printf("Compilation of a shader failed: %s\n", msg);
		free(msg);
		exit(1);
	}
	return id;
}

static uint32_t linkShader(uint32_t vertexID, uint32_t fragmentID) {
	const uint32_t shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexID);
	glAttachShader(shaderID, fragmentID);

	glLinkProgram(shaderID);
	glValidateProgram(shaderID);
	//error handling
	int programCompiled = 0;
	glGetProgramiv(shaderID, GL_LINK_STATUS, &programCompiled);
	if (programCompiled != GL_TRUE)
	{
		int logLength = 0;
		glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msg = malloc(logLength);
		glGetProgramInfoLog(shaderID, logLength, &logLength, msg);
		printf("Linking shader failed: %s\n", msg);
		free(msg);
		exit(1);
	}
	return shaderID;
}

shaderID shader_init(cstr src) {
	shaderFile.open(FilePath);
	if (!shaderFile.is_open()) { std::cout << "Couldn't open shader file. Path: " << FilePath; exit(404); }
	shaderType currType = none;
	std::string currLine;
	std::array<std::string, none> shaderSrc;
	//reading from a shader file
	while (std::getline(shaderFile, currLine)) {
		if (currLine.find("#shader") != std::string::npos)
		{
			if (currLine.find(" vertex") != std::string::npos) currType = vertex;
			else if (currLine.find(" fragment") != std::string::npos) currType = fragment;
			continue;
		}
		if (currType == none) continue;
		 shaderSrc.at(currType) += currLine + "\n"; 
	}
	//compiling and linking shader program
	const uint32_t vertexID = compileShader(vertex, shaderSrc[vertex].c_str());
	const uint32_t fragmentID = compileShader(fragment, shaderSrc[fragment].c_str());
	
	m_ID = linkShader(vertexID, fragmentID, geometryID, computeID, tessControlID, tessEvalID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	if(0 == m_ID) printf("Crating a shader at path: %s failes\n", src);
}


void shader_terminate() {

}

void shader_bind(shaderID) {

}
