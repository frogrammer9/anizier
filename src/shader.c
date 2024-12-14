#include "shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
	uint32_t shaderID = glCreateProgram();

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
	FILE* shaderSrc = fopen(src, "r");
	if(!shaderSrc) {
		printf("Failed to open shader file %s\n", src);
		exit(1);
	}
	fseek(shaderSrc, 0, SEEK_END);
	u32 fileSize = ftell(shaderSrc);
	fseek(shaderSrc, 0, SEEK_SET);
	char* line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	GLenum currType;
	char* vertexSrc = calloc(fileSize + 1, sizeof(char));
	u32 vertexSrcInx = 0;
	char* fragmentSrc = calloc(fileSize + 1, sizeof(char));
	u32 fragmentSrcInx = 0;

	while ((read = getline(&line, &len, shaderSrc)) != -1) {
		if (strstr(line, "#shader vertex")) {
			currType = GL_VERTEX_SHADER;
			continue;
		}
		else if(strstr(line, "#shader fragment")) {
			currType = GL_FRAGMENT_SHADER;
			continue;
		}
		switch(currType) {
			default: continue;
			case GL_VERTEX_SHADER: 
				memcpy(&vertexSrc[vertexSrcInx], line, read);
				vertexSrcInx += read;
			break;
			case GL_FRAGMENT_SHADER: 
				memcpy(&fragmentSrc[fragmentSrcInx], line, read);
				fragmentSrcInx += read;
			break;
		}
	}

	free(line);
	fclose(shaderSrc);

	const uint32_t vertexID = compileShader(vertexSrc, GL_VERTEX_SHADER);
	const uint32_t fragmentID = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);

	free(vertexSrc);
	free(fragmentSrc);
	
	shaderID id = linkShader(vertexID, fragmentID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);

	if(0 == id) printf("Crating a shader at path: %s failed\n", src);
	return id;
}


void shader_terminate(shaderID* id) {
	if(*id) glDeleteProgram(*id);
	*id = 0;
}

void shader_bind(shaderID id) {
	glUseProgram(id);
}
