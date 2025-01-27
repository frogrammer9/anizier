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
void render_cpoints(frame* f, u32 color, shaderID shader, u32 ccurve, u32 colorSelect, u32 lineSelect, controlPoint* cpoint);
void new_frame(animation* anim);
void new_line(frame* f);
controlPoint* add_point(bezierTemplate* curve, f32 xPos, f32 yPos, f32 w);
controlPoint* find_point_if_any(frame* f, vec2 mPos, u32* curveId);
void interpolate_frames(frame* f1, frame* f2, f32 (*easingFunc)(f32), u32 Nframes, frame* framesOUT);
void save_animation(animation* anim, cstr path);
void load_animation(animation* anim, cstr path);
i64 current_time_ms();

#endif
