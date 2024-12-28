#include "editor.h"
#include "altype.h"
#include "app.h"
#include "gui.h"
#include "rendering.h"
#include "window.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easingfunc.h"

typedef f32 (*easing_func_ptr)(f32);

void editor_run(application_hndl* app) {
	bool running = true;
	animation anim = {.frames = NULL, .size = 0, .maxsize = 0};
	new_frame(&anim);
	u32 curveId = -1;
	u32 frameId = 0;
	controlPoint* editedPoint = NULL;
	enum state  {
		none,
		adding_line_f,
		adding_line_s,
		moving_point,
		adding_point,
		interpolating,
	};
	enum state current_state = none;
	enum state inter_state = none;
	u32 inter_framesAmount = 1;
	char cbuffer[128] = "1";
	cstr easing_func[] = {"Linear", "Square", "Root", "S-shaped"};
	easing_func_ptr easing_funcs[] = {easing_linear, easing_sqare, easing_root, easing_s};
	int easing_func_inx = 0;

	while(running && !glfwWindowShouldClose(app->window.win)) {
		switch(current_state) {
			case none:
				if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(app->gui.ctx)) {
					editedPoint = find_point_if_any(&anim.frames[frameId], getMousePosGL(app), &curveId);
					if(editedPoint) current_state = moving_point;
				}
			break;
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
			case moving_point: {
				if(nk_input_is_mouse_released(&app->gui.ctx->input, NK_BUTTON_LEFT)) current_state = none;
				vec2 mousePos = getMousePosGL(app);
				editedPoint->point.x = mousePos.x;
				editedPoint->point.y = mousePos.y;
			}
			break;
			case adding_point:
				if(nk_input_is_mouse_pressed(&app->gui.ctx->input, NK_BUTTON_LEFT) && !nk_item_is_any_active(app->gui.ctx)) {
					vec2 mousePos = getMousePosGL(app);
					add_point(&anim.frames[frameId].curves[curveId], mousePos.x, mousePos.y);
					current_state = none;
					bezierTemplate* curve = &anim.frames[frameId].curves[curveId];
					controlPoint s = curve->points[curve->size-1];
					curve->points[curve->size - 1] = curve->points[curve->size - 2];
					curve->points[curve->size - 2] = s;
				}
			break;
			default: printf("Wydarzyło sie coś nieoczekiwanego :c\n"); break;
		}
		if(app->gui.ctx->input.keyboard.text[app->gui.ctx->input.keyboard.text_len - 1] == '+') ++frameId;
		if(app->gui.ctx->input.keyboard.text[app->gui.ctx->input.keyboard.text_len - 1] == '-') --frameId;
		if(frameId == -1) frameId = 0;
		frameId = MIN(frameId, anim.size - 1);

		GUI_NEW_FRAME(app->gui);
		if(nk_begin(app->gui.ctx, "Menu", nk_rect(50, 50, 300, 400), NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE)) {
			nk_layout_row_static(app->gui.ctx, 30, 200, 1);
			nk_labelf(app->gui.ctx, NK_TEXT_LEFT, "Frame: %u/%u (+/-)", frameId + 1, anim.size);
			if(inter_state == interpolating) {
				nk_layout_row_static(app->gui.ctx, 20, 200, 1);
				nk_label(app->gui.ctx, "Amount of frames:", NK_TEXT_LEFT);
				if(nk_edit_string_zero_terminated(app->gui.ctx,NK_EDIT_FIELD, cbuffer, sizeof(cbuffer), nk_filter_decimal)) {
					sscanf(cbuffer, "%u", &inter_framesAmount);
				}
				nk_layout_row_static(app->gui.ctx, 20, 200, 1);
				easing_func_inx = nk_combo(app->gui.ctx, easing_func, sizeof(easing_func) / sizeof(char*), easing_func_inx, 30, nk_vec2(200, 200));
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "Interpolate")) {
					inter_state = none;
					for(u32 i = 0; i < inter_framesAmount; ++i) {
						new_frame(&anim);
					}
					frame tframe = anim.frames[frameId];
					interpolate_frames(&anim.frames[frameId - 1], &tframe, easing_funcs[easing_func_inx], inter_framesAmount, &anim.frames[frameId]);
					frameId += inter_framesAmount;
					anim.frames[frameId] = tframe;
				}
			}
			else {
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "New line")) {
					new_line(&anim.frames[frameId]);
					curveId = anim.frames[frameId].size - 1;
					current_state = adding_line_f;
				}
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "Add point to selected line")) {
					current_state = adding_point;
				}
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "New frame")) {
					new_frame(&anim);
					curveId = -1;
					++frameId;
				}
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "Create an interpolation")) {
					memset(cbuffer, 0, sizeof(cbuffer));
					cbuffer[0] = '1';
					new_frame(&anim);
					curveId = -1;
					++frameId;
					inter_state = interpolating;
					anim.frames[frameId].size = anim.frames[frameId - 1].size;
					anim.frames[frameId].maxsize = anim.frames[frameId - 1].maxsize;
					anim.frames[frameId].curves = malloc(anim.frames[frameId].maxsize * sizeof(bezierTemplate));
					for(u32 i = 0; i < anim.frames[frameId].size; ++i) {
						anim.frames[frameId].curves[i].size = anim.frames[frameId - 1].curves[i].size;
						anim.frames[frameId].curves[i].maxsize = anim.frames[frameId - 1].curves[i].maxsize;
						anim.frames[frameId].curves[i].points = malloc(anim.frames[frameId].curves[i].maxsize * sizeof(controlPoint));
						memcpy(	anim.frames[frameId].curves[i].points, 
								anim.frames[frameId - 1].curves[i].points, 
								anim.frames[frameId - 1].curves[i].size * sizeof(controlPoint));
					}
				}
				nk_layout_row_static(app->gui.ctx, 60, 200, 1);
				if(nk_button_label(app->gui.ctx, "Run animation")) {
					printf("Button pressed\n");
				}
			}
			nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			if(nk_button_label(app->gui.ctx, "Back")) {
				running = false;
			}
			//nk_layout_row_static(app->gui.ctx, 60, 200, 1);
			//if(nk_button_label(app->gui.ctx, "Debug print anim")) {
			//	for(u32 i = 0; i < anim.size; ++i) {
			//		printf("frame: %u - size: %u - maxsize: %u\n", i, anim.frames[i].size, anim.frames[i].maxsize);
			//		for(u32 j = 0; j < anim.frames[i].size; ++j) {
			//		printf("  curve: %u - size: %u - maxsize: %u\n", j, anim.frames[i].curves[j].size, anim.frames[i].curves[j].maxsize);
			//			for(u32 k = 0; k < anim.frames[i].curves[j].size; ++k) {
			//				printf("    {%f, %f}\n", anim.frames[i].curves[j].points[k].point.x, anim.frames[i].curves[j].points[k].point.y);
			//			}
			//		}
			//	}
			//}
		}
		nk_end(app->gui.ctx);
		GUI_RENDER(app->gui);

		rnBuffer lineBuffer;
		rnBuffer_init(&lineBuffer, false);
		for(u32 i = 0; i < anim.frames[frameId].size; ++i) {
			sample buffer[100];
			generate_bezier_samples(anim.frames[frameId].curves[i].points, anim.frames[frameId].curves[i].size, 50, buffer);
			rnBuffer_add_curve(&lineBuffer, buffer, 50);
		}
		u32 ftr = rnBuffer_new_frame(&lineBuffer);
		rnBuffer_render(&lineBuffer, app->lineShader, ftr, 0);
		rnBuffer_terminate(&lineBuffer);

		render_cpoints(&anim.frames[frameId], 0x19e023ff, app->pointShader, curveId, 0xcc1dc3ff);

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
	curve->points[curve->size - 1].weight = 1.f;
}

static u32 len(controlPoint cp, vec2 mp) {
	i64 X = cp.point.x - mp.x;
	i64 Y = cp.point.y - mp.y;
	return sqrt(X * X + Y * Y);
}

controlPoint* find_point_if_any(frame* f, vec2 mPos, u32* curveId) {
	for(u32 i = 0; i < f->size; ++i) {
		for(u32 j = 0; j < f->curves[i].size; ++j) {
			if(len(f->curves[i].points[j], mPos) < 10.f) {
				*curveId = i;
				return &f->curves[i].points[j];
			}
		}
	}
	return NULL;
}

void render_cpoints(frame* f, u32 color, shaderID shader, u32 ccurve, u32 colorSelect) {
	u32 size = 0;
	for(u32 i = 0; i < f->size; ++i) size += f->curves[i].size;
	sample* samples = malloc(size * sizeof(sample));
	u32 inx = 0;
	for(u32 i = 0; i < f->size; ++i) {
		for(u32 j = 0; j < f->curves[i].size; ++j) {
			samples[inx].pos.x = f->curves[i].points[j].point.x;
			samples[inx].pos.y = f->curves[i].points[j].point.y;
			if(i == ccurve) samples[inx++].col = colorSelect;
			else samples[inx++].col = color;
		}
	}
	render_points(samples, size, shader);
}

void interpolate_frames(frame* f1, frame* f2, f32 (*easingFunc)(f32), u32 Nframes, frame* framesOUT) {
	if(f1->size != f2->size) {
		printf("ERROR Tried to interpolate between two frames with different amount of control points\n"); return;
	}
	for(u32 i = 0; i < Nframes; ++i) {
		framesOUT[i].size = f1->size;
		framesOUT[i].maxsize = f1->maxsize;
		framesOUT[i].curves = malloc(f1->maxsize * sizeof(bezierTemplate));
		for(u32 j = 0; j < f1->size; ++j) {
			framesOUT[i].curves[j].size = f1->curves[j].size;
			framesOUT[i].curves[j].maxsize = f1->curves[j].maxsize;
			framesOUT[i].curves[j].points = malloc(f1->curves[j].maxsize * sizeof(controlPoint));
		}
	}
	for(u32 fId = 0; fId < Nframes; ++fId) {
		f32 easingFactor = easingFunc((f32)(fId + 1) / (Nframes + 1));
		easingFactor = CLAMP(easingFactor, 0.f, 1.f);
		for(u32 i = 0; i < f1->size; ++i) {
			for(u32 j = 0; j < f1->curves[i].size; ++j) {
				vec2 p0 = f1->curves[i].points[j].point;
				vec2 p1 = f2->curves[i].points[j].point;
				framesOUT[fId].curves[i].points[j].point.x = p0.x + (p1.x - p0.x) * easingFactor;
				framesOUT[fId].curves[i].points[j].point.y = p0.y + (p1.y - p0.y) * easingFactor;
				framesOUT[fId].curves[i].points[j].weight = 1.f;
			}
		}
	}
}
