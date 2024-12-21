#ifndef EDITOR_H
#define EDITOR_H

#include "app.h"
#include "rendering.h"

typedef struct {
	controlPoint* points;
	u32 size;
	u32 maxsize;
} bezierTemplate;

typedef struct {
	bezierTemplate* curves;
	u32 size;
	u32 maxsize;
} frame;

typedef struct {
	frame* frames;
	u32 size;
	u32 maxsize;
} animation;

void editor_run(application_hndl* app);
void render_cpoints(bezierTemplate* curve, shaderID shader);
void new_frame(animation* anim);
void new_line(frame* f);
void new_pdata(bezierTemplate* curve);

#endif
