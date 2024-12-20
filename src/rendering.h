#ifndef RENDERING_H
#define RENDERING_H

#include "altype.h"
#include "shader.h"

typedef struct {
	u32 VAO, VBO, EBO;
	u64 size;
	u64 maxsize;
	u32* elementsPerFrame;
	u32 elementVal;
	u32 samplesInxEnd;
	u32 samplesMaxSize;
	u32 frameCount;
	bool dynamic;
} rnBuffer;

typedef struct {
	vec2 point;
	f32 weight;
} controlPoint;

typedef struct {
	vec2 pos;
	u32 col;
} sample;

void generate_bezier_samples(controlPoint* cps, u32 count, u32 sampleAmount, sample* samplesOUT);
u32 rnBuffer_add_curve(rnBuffer* buff, sample* samples, u32 sampleAmount);
void rnBuffer_edit_curve(rnBuffer* buff, sample* samples, u32 sampleAmount, u32 curveID);
u32 rnBuffer_new_frame(rnBuffer* buff);
void rnBuffer_init(rnBuffer* buff, bool dynamic);
void rnBuffer_terminate(rnBuffer* buff);
void rnBuffer_alloc(rnBuffer* buff, u32 size); // Make sure that there is a place for at least "size" elements in the buffer
void rnBuffer_render(rnBuffer* buff, shaderID shader, u32 frameID, u32 fps); // If frameID == 0 render all frames else fps is ignored

#endif
