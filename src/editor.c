#include "editor.h"
#include "app.h"
#include "gui.h"
#include "rendering.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void editor_run(application_hndl* app) {
	bool running = true;
	animation anim = {.frames = NULL, .size = 0, .maxsize = 0};
	new_frame(&anim);
	u32 curveId = -1;
	u32 frameId = 0;
	enum state  {
		none,
		adding_line_f,
		adding_line_s,
	};
	enum state current_state = none;

	while(running) {
		switch(current_state) {
			case none: break;
			case adding_line_f: {
				if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(app->gui.ctx)) {
					vec2 mousePos = getMousePosGL(app);
					add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y);
					current_state = adding_line_s;
				}
			}
			break;
			case adding_line_s: {
				if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(app->gui.ctx)) {
					vec2 mousePos = getMousePosGL(app);
					add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y);
					current_state = none;
				}
			}
			break;
		}

		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			nk_labelf(app->gui.ctx, NK_TEXT_LEFT, "Current curve: %u", curveId);
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New line")) {
				printf("Button pressed\n");
				new_line(&anim.frames[frameId]);
				++curveId;
				current_state = adding_line_f;
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New frame")) {
				printf("Button pressed\n");
				new_frame(&anim);
				++frameId;
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Run animation")) {
				printf("Button pressed\n");
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Back")) {
				running = false;
			}

		}
		nk_end(app->gui.ctx);
		GUI_RENDER(app->gui);

		render_cpoints(&anim.frames[frameId], 0xff0000ff, app->pointShader);

		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	for(u32 i = 0; i < anim.size; ++i) {
		for(u32 j = 0; j < anim.frames[i].size; ++j) {
			free(anim.frames[i].curves[j].points);
		}
		free(anim.frames[i].curves);
	}
	free(anim.frames);

}

void new_frame(animation* anim) {
	if(anim->maxsize == 0) {
		anim->frames = malloc(2 * sizeof(frame));
		anim->size = 0;
		anim->maxsize = 2;
	}
	if(++anim->size > anim->maxsize) {
		anim->frames = realloc(anim->frames, anim->maxsize * 2 * sizeof(frame));
		anim->maxsize *= 2;
	}
	anim->frames[anim->size - 1].curves = NULL;
	anim->frames[anim->size - 1].size = 0;
	anim->frames[anim->size - 1].maxsize = 0;
}

void new_line(frame* f) {
	if(f->maxsize == 0) {
		f->curves = malloc(2 * sizeof(bezierTemplate));
		f->size = 0;
		f->maxsize = 2;
	}
	if(++f->size > f->maxsize) {
		f->curves = realloc(f->curves, f->maxsize * 2 * sizeof(bezierTemplate));
		f->maxsize *= 2;
	}
	printf("Size: %u, Maxsize: %u\n", f->size, f->maxsize);
	f->curves[f->size - 1].size = 0;
	f->curves[f->size - 1].maxsize = 0;
	f->curves[f->size - 1].points = NULL;
}

void add_point(bezierTemplate* curve, f32 xPos, f32 yPos) {
	if(curve->maxsize == 0) {
		curve->points = malloc(2 * sizeof(controlPoint));
		curve->maxsize = 2;
	}
	if(++curve->size > curve->maxsize) {
		curve->points = realloc(curve->points, curve->maxsize * 2 * sizeof(controlPoint));
		curve->maxsize *= 2;
	}
	curve->points[curve->size - 1].point.x = xPos;
	curve->points[curve->size - 1].point.y = yPos;
}

void render_cpoints(frame* f, u32 color, shaderID shader) {
	u32 size = 0;
	for(u32 i = 0; i < f->size; ++i) size += f->curves[i].size;
	sample* samples = malloc(size * sizeof(sample));
	u32 inx = 0;
	for(u32 i = 0; i < f->size; ++i) {
		for(u32 j = 0; j < f->curves[i].size; ++j) {
			samples[inx].pos.x = f->curves[i].points[j].point.x;
			samples[inx].pos.y = f->curves[i].points[j].point.y;
			samples[inx++].col = color;
		}
	}
	render_points(samples, size, shader);
}
