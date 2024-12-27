#include "rendering.h"
#include "shader.h"
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define DEBUG 0
#define DEBUG_OVERLOAD true

#define S2ES(size) ((size) ? (2 * (size - 1)) : (0)) //Calculates the amount of indexes based on the vertex count
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

static u64 factorial(u64 in) {
	static u64* cashe = NULL;
	static u64 cashe_size = 0;
	if(cashe == NULL) {
		cashe = malloc(2 * sizeof(u64));
		cashe_size = 2;
		cashe[0] = 1;
		cashe[1] = 1;
	}
	if(in < cashe_size) return cashe[in];
	cashe = realloc(cashe, (in + 1) * sizeof(u64));
	u64 old_cash_size = cashe_size;
	cashe_size = in + 1;
	for(; old_cash_size <= in; ++old_cash_size) cashe[old_cash_size] = old_cash_size * cashe[old_cash_size - 1];
	return cashe[in];
}

static u64 stupid_factorial(u64 in) {
	u64 out = 1;
	for(u64 i = 2; i <= in; ++i) out *= i;
	return out;
}

static u64 binomialCoefficient(u64 n, u64 i) {
	return factorial(n) / (factorial(i) * factorial(n - i));
}

static u64 stupid_binomialCoefficient(u64 n, u64 i) {
	return stupid_factorial(n) / (stupid_factorial(i) * stupid_factorial(n - i));
}

static f64 bernsteinPolinomial(u64 n, u64 i, f32 t) {
	return stupid_binomialCoefficient(n, i) * pow(1 - t, n - i) * pow(t, i);
}

void generate_bezier_samples(controlPoint* cps, u32 count, u32 sampleAmount, sample* samplesOUT) {
	f32 t = 0.f;
	sample point;
	point.col = 0x0000ffff;
	for(u32 k = 0; k < sampleAmount; ++k) {
		point.pos.x = 0.f;
		point.pos.y = 0.f;
		f32 weight = 0.f;
		t = (f32)k / (sampleAmount - 1);
		for(u32 i = 0; i < count; ++i) {
			float bern = bernsteinPolinomial(count - 1, i, t);
			point.pos.x += bern * cps[i].point.x * cps[i].weight;
			point.pos.y += bern * cps[i].point.y * cps[i].weight;
			weight += bern * cps[i].weight;
		}
		point.pos.x /= weight;
		point.pos.y /= weight;
		samplesOUT[k] = point;
	}
}

void rnBuffer_init(rnBuffer* buff, bool dynamic) {
	buff->VAO = 0; buff->VBO = 0; buff->EBO = 0;
	buff->size = 0; buff->maxsize = 0;
	buff->elementsPerFrame = NULL; buff->verticesPerFrame = NULL; buff->samplesInxEnd = 0; buff->samplesMaxSize = 64;
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
	buff->verticesPerFrame = calloc(buff->samplesMaxSize, sizeof(u32));
}

void rnBuffer_terminate(rnBuffer* buff) {
	glDeleteBuffers(1, &buff->VBO);
	glDeleteBuffers(1, &buff->EBO);
	glDeleteVertexArrays(1, &buff->VAO);
	free(buff->elementsPerFrame);
}

void rnBuffer_alloc(rnBuffer* buff, u32 size) {
	if(buff->maxsize > buff->size + size) return;
	if(buff->maxsize == 0)	buff->maxsize = 128;
	while(buff->maxsize < buff->size + size) {
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

	glBufferSubData(GL_ARRAY_BUFFER, 0, buff->size * sizeof(sample), vertexBuff);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, S2ES(buff->size) * sizeof(u32), elementBuff);

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
	u32 elementBuff[S2ES(sampleAmount)];
	for(u32 i = 0; i < sampleAmount - 1; ++i) {
		elementBuff[2 * i] = buff->elementVal;
		elementBuff[2 * i + 1] = ++buff->elementVal;
	}
	++buff->elementVal;
	u32 elementOffset = 0;
	for(u32 i = 0; i <= buff->samplesInxEnd; ++i) elementOffset += buff->elementsPerFrame[i];
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, elementOffset * sizeof(u32), S2ES(sampleAmount) * sizeof(u32), elementBuff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	buff->elementsPerFrame[buff->samplesInxEnd] += S2ES(sampleAmount);
	buff->verticesPerFrame[buff->samplesInxEnd] += sampleAmount;
	return curveID;
}

void rnBuffer_edit_curve(rnBuffer* buff, sample* samples, u32 sampleAmount, u32 curveID) {
	u64 size = 0;
	for(u32 i = 0; i < buff->frameCount; ++i) {
		size += buff->verticesPerFrame[i];
		if(size == curveID) {
			size = buff->verticesPerFrame[i + 1];
			break;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, buff->VBO);
	if(size > sampleAmount) {
		printf("Passing less samples then required to edit a curve\n");
	}
	glBufferSubData(GL_ARRAY_BUFFER, curveID * sizeof(sample), MIN(size, sampleAmount) * sizeof(sample), samples);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

u32 rnBuffer_new_frame(rnBuffer* buff) {
	buff->samplesInxEnd++;
	if(buff->samplesInxEnd == buff->samplesMaxSize) {
		buff->elementsPerFrame = realloc(buff->elementsPerFrame, buff->samplesMaxSize * 2 * sizeof(u32));
		buff->verticesPerFrame = realloc(buff->verticesPerFrame, buff->samplesMaxSize * 2 * sizeof(u32));
		buff->samplesMaxSize *= 2;
	}
	return buff->samplesInxEnd;
}

void rnBuffer_render(rnBuffer* buff, shaderID shader, u32 frameID, u32 fps) {
	#if DEBUG > 0
	static u32 lastframe;
	if(frameID != lastframe || DEBUG_OVERLOAD) {
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
		for(u32 i = 0; i < buff->size; ++i) printf("Vertex: pos: {%f, %f}, color: 0x%08x\n", vBuff[i].pos.x, vBuff[i].pos.y, vBuff[i].col);
		u32 elementOffset = 0;
		for(u32 i = 0; i < frameID - 1; ++i) elementOffset += buff->elementsPerFrame[i];
		for(u32 i = 0; i < buff->elementsPerFrame[frameID - 1]; i += 2) printf("Element: %u, %u\n", eBuff[elementOffset + i], eBuff[elementOffset + i + 1]);
		printf("Elements to be rendered: %u\n", buff->elementsPerFrame[frameID - 1]);
		printf("Offset: %u\n", elementOffset);
		free(vBuff);
		free(eBuff);
	}
	lastframe = frameID;
	#endif


	if(frameID) {
		u64 offset = 0;
		for(u32 i = 0; i < frameID - 1; ++i) { offset += buff->elementsPerFrame[i]; }
		glBindVertexArray(buff->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buff->EBO);
		shader_bind(shader);
		glDrawElements(GL_LINES, buff->elementsPerFrame[frameID - 1], GL_UNSIGNED_INT, (void*)(offset * sizeof(u32)));
		shader_bind(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return;
	}
	printf("Rendering all frames is not implemented yet bcoz multithreading in C is scary\n");
	//TODO: Implement rendering xd 
}

void render_points(sample* samples, u32 sampleAmount, shaderID shader) {
	u32 vbo, vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sampleAmount * sizeof(sample), samples, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(sample), (void*)0); // vec2 position
	glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(sample), (void*)(2 * sizeof(f32))); // u32 color
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader_bind(shader);
	glDrawArrays(GL_POINTS, 0, sampleAmount);
	shader_bind(0);

	glBindBuffer(GL_VERTEX_ARRAY, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &vbo);
}

