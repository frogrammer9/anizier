#include "rendering.h"
#include "shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

#define S2ES(size) ((size) ? (2 * (size - 1)) : (0)) //Calculates the amount of indexes based on the vertex count

void generate_bezier_samples(controlPoint* cps, u32 count, u32 sampleAmount, sample* samplesOUT) {

}

void rnBuffer_init(rnBuffer* buff, bool dynamic) {
	buff->VAO = 0; buff->VBO = 0; buff->EBO = 0;
	buff->size = 0; buff->maxsize = 0;
	buff->elementsPerFrame = NULL; buff->samplesInxEnd = 0; buff->samplesMaxSize = 64;
	buff->frameCount = 0;
	buff->dynamic = dynamic;
	buff->elementVal = 0;

	glGenVertexArrays(1, &buff->VAO);
	glBindVertexArray(buff->VAO);

	glGenBuffers(1, &buff->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glGenBuffers(1, &buff->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, (dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sample), (void*)0); // vec2 position
	glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(sample), (void*)(2 * sizeof(f32))); // u32 color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	buff->elementsPerFrame = calloc(buff->samplesMaxSize, sizeof(u32));
}

void rnBuffer_terminate(rnBuffer* buff) {
	free(buff->elementsPerFrame);
}

void rnBuffer_alloc(rnBuffer* buff, u32 size) {
	if(buff->maxsize > size) return;
	if(buff->maxsize == 0)	buff->maxsize = 128;
	while(buff->maxsize < size) {
		buff->maxsize *= 2;
	}

	#if DEBUG == 1
	printf("Buffer realloc to size %lu\n", buff->maxsize);
	#endif

	sample* vertexBuff = malloc(buff->size * sizeof(sample));
	u32* elementBuff = malloc(S2ES(buff->size) * sizeof(u32));

	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);

	glGetBufferSubData(GL_ARRAY_BUFFER, 0, buff->size * sizeof(sample), vertexBuff);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, S2ES(buff->size) * sizeof(u32), elementBuff);

	glBufferData(GL_ARRAY_BUFFER, buff->maxsize * sizeof(sample), NULL, (buff->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, S2ES(buff->maxsize) * sizeof(u32), NULL, (buff->dynamic) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, buff->size, vertexBuff);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, S2ES(buff->size), elementBuff);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	free(vertexBuff);
	free(elementBuff);
}

u32 rnBuffer_add_curve(rnBuffer* buff, sample* samples, u32 sampleAmount) {
	rnBuffer_alloc(buff, sampleAmount);
	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
	u32 curveID = buff->size;
	glBufferSubData(GL_ARRAY_BUFFER, curveID * sizeof(sample), sampleAmount * sizeof(sample), samples);
	buff->size += sampleAmount;
	u32* elementBuff = malloc(S2ES(sampleAmount) * sizeof(u32));
	for(u32 i = 0; i < sampleAmount - 1; ++i) { //PERF: This could be generated once and sampled (regenerated if a larger line draw request happens)
		elementBuff[2 * i] = buff->elementVal++;
		elementBuff[2 * i + 1] = buff->elementVal;
	}
	buff->elementVal++;
	u32 elementOffset = 0;
	for(u32 i = 0; i <= buff->samplesInxEnd; ++i) elementOffset += buff->elementsPerFrame[i];
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, elementOffset * sizeof(u32), S2ES(sampleAmount) * sizeof(u32), elementBuff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	buff->elementsPerFrame[buff->samplesInxEnd] += S2ES(sampleAmount);
	free(elementBuff);
	return curveID;
}

void rnBuffer_edit_curve(rnBuffer* buff, sample* samples, u32 sampleAmount, u32 curveID) {
	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBufferSubData(GL_ARRAY_BUFFER, curveID, sampleAmount * sizeof(sample), samples);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void rnBuffer_new_frame(rnBuffer* buff) {
	buff->samplesInxEnd++;
	if(buff->samplesInxEnd == buff->samplesMaxSize) {
		u32* newArr = calloc(buff->samplesMaxSize * 2, sizeof(u32)); //TODO: change to realloc
		buff->samplesMaxSize *= 2;
		memcpy(newArr, buff->elementsPerFrame, (buff->samplesInxEnd + 1) * sizeof(u32));
		free(buff->elementsPerFrame);
		buff->elementsPerFrame = newArr;
	}
}

void rnBuffer_render(rnBuffer* buff, shaderID shader, u32 frameID, u32 fps) {
	#if DEBUG == 1
	sample* vBuff = malloc(buff->size * sizeof(sample));
	u32* eBuff = malloc(S2ES(buff->size) * sizeof(u32));
	GLsizei vBuffS = 0;
	GLsizei eBuffS = 0;
	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vBuffS);
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &eBuffS);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, buff->size * sizeof(sample), vBuff);
	glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, S2ES(buff->size) * sizeof(u32), eBuff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	printf("Vertex size: %lu. Element size: %lu\n", vBuffS / sizeof(sample), eBuffS / sizeof(u32));
	for(u32 i = 0; i < buff->size; ++i) printf("Vertex: pos: {%f, %f}, color: %x\n", vBuff[i].pos.x, vBuff[i].pos.y, vBuff[i].col);
	for(u32 i = 0; i < buff->size - 1 && 2 * i < buff->elementsPerFrame[frameID - 1]; ++i) printf("Element: %u, %u\n", eBuff[2 * i], eBuff[2 * i + 1]);
	printf("Elements to be rendered: %u\n", buff->elementsPerFrame[frameID - 1]);
	free(vBuff);
	free(eBuff);
	#endif
	if(frameID) {
		u64 offset = 0;
		for(u32 i = 0; i < frameID - 1; ++i) { offset += buff->elementsPerFrame[i]; }
		glBindVertexArray(buff->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
		shader_bind(shader);
		glDrawElements(GL_LINES, buff->elementsPerFrame[frameID - 1], GL_UNSIGNED_INT, (void*)offset);
		shader_bind(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return;
	}
	printf("Rendering all frames is not implemented yet bcoz multithreading in C is scary\n");
	//TODO: Implement rendering xd 
}

