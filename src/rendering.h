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
} rnBuffer;

typedef struct {
	vec2 point;
	f32 weight;
} controlPoint;

void generate_bezier_samples(controlPoint* cps, u32 count, u32 sampleAmount, vec2* samplesOUT);

u32 rnBuffer_add_curve(rnBuffer* buff, vec2* samples, u32 sampleAmount);

void rnBuffer_edit_curve(rnBuffer* buff, vec2* samples, u32 curveID);

void rnBuffer_new_frame(rnBuffer* buff);



#endif
