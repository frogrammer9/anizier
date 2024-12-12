#ifndef RENDERING_H
#define RENDERING_H

#include "altype.h"

typedef struct {
	u32 VAO, VBO, EBO;
	u64 size;
	u64 maxsize;
	u32* samplesPerFrame;
	u32 samplesSize;
	u32 samplesMaxSize;
	u32 frameCount;
	bool dynamic;
} rnBuffer;

typedef struct {
	vec2 point;
	f32 weight;
} controlPoint;

void generate_bezier_samples(controlPoint* cps, u32 count, u32 sampleAmount, vec2* samplesOUT);

u32 rnBuffer_add_curve(rnBuffer* buff, vec2* samples, u32 sampleAmount);

void rnBuffer_edit_curve(rnBuffer* buff, vec2* samples, u32 curveID);

void rnBuffer_new_frame(rnBuffer* buff);

void rnBuffer_init(rnBuffer* buff, bool dynamic);

void rnBuffer_terminate(rnBuffer* buff);

void rnBuffer_alloc(rnBuffer* buff, u32 size); // Make sure that there is a place for at least "size" elements in the buffer

void rnBuffer_render(rnBuffer* buff, u32 frameID, u32 fps); // If frameID == 0 render all frames else fps is ignored


#endif
