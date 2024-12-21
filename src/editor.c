#include "editor.h"
#include "app.h"
#include "gui.h"
#include "rendering.h"
#include "window.h"
#include <stdio.h>
#include <stdlib.h>

void editor_run(application_hndl* app) {
	bool running = true;
	animation anim = {.frames = NULL, .size = 0, .maxsize = 0};
	new_frame(&anim);
	anim.frames[0].curves = NULL; 
	anim.frames[0].size = 0; 
	anim.frames[0].maxsize = 0; 
	u32 curveId = 0;
	u32 frameId = 0;
	enum state  {
		none,
		adding_line_f,
		adding_line_s,
	};
	enum state current_state = none;
	while(running) {
		switch(current_state) {
		case none:
		break;
		case adding_line_f:
			if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT)) {
					anim.frames[frameId].curves[curveId].points[0].point = getMousePosGL(app);
					anim.frames[frameId].curves[curveId].size = 1;
					current_state = adding_line_s;
				}
		break;
		case adding_line_s:
			if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT)) {
					anim.frames[frameId].curves[curveId].points[1].point = getMousePosGL(app);
					anim.frames[frameId].curves[curveId].size = 2;
					current_state = none;
				}
		break;
		}

		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_text(app->gui.ctx, "Test", 0, 0);
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New line")) {
				printf("Button pressed\n");
				new_line(&anim.frames[frameId]);
				current_state = adding_line_f;
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "New frame")) {
				printf("Button pressed\n");
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

		if(anim.frames[frameId].curves && anim.frames[frameId].curves->size > 0)
			render_cpoints(&anim.frames[frameId].curves[curveId], app->pointShader);

		window_FEP(&app->window);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	free(anim.frames);

}

void new_frame(animation* anim) {
	if(anim->maxsize == 0) {
		anim->frames = malloc(2 * sizeof(frame));
		anim->size = 0;
		anim->maxsize = 2;
	}
	if(anim->size++ > anim->maxsize) {
		anim->frames = realloc(anim->frames, anim->maxsize * 2 * sizeof(frame));
		anim->maxsize *= 2;
	}
	anim->frames[anim->size - 1].curves = NULL;
	anim->frames[anim->size - 1].size = 0;
	anim->frames[anim->size - 1].maxsize = 0;
}

void new_line(frame* f) {
	if(f->maxsize == 0) {
		f->curves = malloc(2 * sizeof(frame));
		f->size = 0;
		f->maxsize = 2;
	}
	if(f->size++ > f->maxsize) {
		f->curves = realloc(f->curves, f->maxsize * 2 * sizeof(frame));
		f->maxsize *= 2;
	}
	new_pdata(&f->curves[f->size - 1]);
}

void new_pdata(bezierTemplate *curve) {
	if(curve->maxsize == 0) {
		curve->points = malloc(2 * sizeof(sample));
		curve->size = 0;
		curve->maxsize = 2;
	}
	if(curve->size++ > curve->maxsize) {
		curve->points = realloc(curve->points, curve->maxsize * 2 * sizeof(sample));
		curve->maxsize *= 2;
	}
}

void render_cpoints(bezierTemplate* curve, shaderID shader) {
	sample samples[curve->size];
	for(u32 i = 0; i < curve->size; ++i) {
		samples[i].pos = curve->points[i].point;
		samples[i].col = 0xff0000ff;
	}
	render_points(samples, curve->size, shader);
}
