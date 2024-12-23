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
void render_cpoints(frame* f, u32 color, shaderID shader);
void new_frame(animation* anim);
void new_line(frame* f);
void add_point(bezierTemplate* curve, f32 xPos, f32 yPos);
controlPoint* find_point_if_any(frame* f, vec2 mPos);

#endif
