#include "rendering.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

void rnBuffer_init(rnBuffer* buff, bool dynamic) {
	buff->VAO = 0; buff->VBO = 0; buff->EBO = 0;
	buff->size = 0; buff->maxsize = 0;
	buff->samplesPerFrame = NULL; buff->samplesSize = 0; buff->samplesMaxSize = 64;
	buff->frameCount = 0;
	buff->dynamic = dynamic;

	glGenVertexArrays(1, &buff->VAO);
	glBindVertexArray(buff->VAO);

	glGenBuffers(1, &buff->EBO);
	glBindBuffer(buff->EBO, GL_ELEMENT_ARRAY_BUFFER);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glGenBuffers(1, &buff->VBO);
	glBindBuffer(buff->VBO, GL_ARRAY_BUFFER);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)0); // vec2 position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)2); // vec2 color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	buff->samplesPerFrame = calloc(buff->samplesMaxSize, sizeof(u32));
}

void rnBuffer_terminate(rnBuffer* buff) {
	free(buff->samplesPerFrame);
}

void rnBuffer_alloc(rnBuffer* buff, u32 size) {
	if(buff->maxsize > size) return;
	if(buff->maxsize == 0)	buff->maxsize = 128;
	while(buff->maxsize < size) {
		buff->maxsize *= 2;
	}

	f32* vertexBuff = malloc(buff->size * 4 * sizeof(f32));
	u32* elementBuff = malloc(buff->size * sizeof(u32));

	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, buff->size * 4 * sizeof(f32), vertexBuff);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, buff->size * sizeof(u32), elementBuff);
	glBufferData(GL_ARRAY_BUFFER, buff->maxsize * 4 * sizeof(f32), vertexBuff, (buff->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buff->maxsize * sizeof(u32), elementBuff, (buff->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	free(vertexBuff);
	free(elementBuff);
}

u32 rnBuffer_add_curve(rnBuffer* buff, vec2* samples, u32 sampleAmount) {

}

void rnBuffer_edit_curve(rnBuffer* buff, vec2* samples, u32 curveID) {

}

void rnBuffer_new_frame(rnBuffer* buff) {

}

void rnBuffer_render(rnBuffer* buff, u32 frameID, u32 fps) {
	if(frameID) {
		u64 offset = 0;
		for(u32 i = 0; i < frameID; ++i) { offset += buff->samplesPerFrame[i]; }
		glBindVertexArray(buff->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
		glDrawElements(GL_LINES, buff->samplesPerFrame[frameID], GL_UNSIGNED_INT, (void*)offset);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return;
	}
	printf("Rendering all frames is not implemented yet bcoz multithreading in C is scary\n");
	//TODO: Implement rendering xd 
}
